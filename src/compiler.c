#include <Python.h>
#include <PyHP.h>

int PyHP_CompileCell(PyHP_PageCode_Cell *dst, int py_version, PyHP_StatementType type, const char *source, Py_ssize_t start, Py_ssize_t len, int optimize)
{
    switch (type)
    {
        case PyHP_StatementType_RAW:
            dst->type = PyHP_PageCodeType_TEXT;
            dst->value.raw.start = start;
            dst->value.raw.len = len;
            return 0;
        case PyHP_StatementType_BLOCK:
            if (py_version == 3)
            {
                return PyHP_CompileCell(dst, py_version, PyHP_StatementType_BLOCK3, source, start, len, optimize);
            }
        case PyHP_StatementType_INLINE:
            if (py_version == 3)
            {
                return PyHP_CompileCell(dst, py_version, PyHP_StatementType_INLINE3, source, start, len, optimize);
            }
        case PyHP_StatementType_BLOCK2:
        case PyHP_StatementType_BLOCK1:
        case PyHP_StatementType_INLINE2:
        case PyHP_StatementType_INLINE1:
            PyErr_Format(PyExc_NotImplementedError, "support of lower python versions not implemented yet");
            return 1;
        case PyHP_StatementType_INLINE3:
#if PY_VERSION_HEX >= 0x03020000
            dst->value.code = Py_CompileStringExFlags(source + start, "<py3hp page inline insertion>", Py_eval_input, NULL, optimize);
#else
            dst->value.code = Py_CompileString(source + start, "<py3hp page inline insertion>", Py_eval_input);
#endif
            if (dst->value.code == NULL)
            {
                return 1;
            }
            dst->type = PyHP_PageCodeType_EVAL;
            return 0;
        case PyHP_StatementType_BLOCK3:
#if PY_VERSION_HEX >= 0x03020000
            dst->value.code = Py_CompileStringExFlags(source + start, "<py3hp page block insertion>", Py_file_input, NULL, optimize);
#else
            dst->value.code = Py_CompileString(source + start, "<py3hp page block insertion>", Py_file_input);
#endif
            if (dst->value.code == NULL)
            {
                return 1;
            }
            dst->type = PyHP_PageCodeType_EXEC;
            return 0;
        default:
            PyErr_BadInternalCall();
            return 1;
    }
}

int PyHP_Compile(const char *src, Py_ssize_t len, Py_ssize_t *p_slen, PyHP_PageCode_Cell **p_statements, Py_ssize_t *p_blen, char **p_buffer, int optimize)
{
    PyHP_ParserIteratorState iterator;
    PyHP_ParserMatch match;
    char *dst;
    Py_ssize_t new_len;
    Py_ssize_t pos;
    Py_ssize_t st_pos;
    Py_ssize_t st_len;
    PyHP_PageCode_Cell *statements;
    Py_ssize_t statements_len;
    Py_ssize_t statements_pos;

    new_len = 0;
    pos = 0;
    dst = PyMem_Malloc(len);
    if (dst == NULL)
    {
        PyErr_NoMemory();
        return 1;
    }

    statements_len = 8;
    statements_pos = 0;
    statements = PyMem_Malloc(sizeof(PyHP_PageCode_Cell) * statements_len);
    if (statements == NULL)
    {
        PyMem_Free(dst);
        PyErr_NoMemory();
        return 1;
    }


    PyHP_Parser_Init(src, len, &iterator);
    while ((match = PyHP_Parser_Next(src, len, &iterator)).type != PyHP_StatementType_NONE)
    {
        if (statements_pos >= statements_len)
        {
            statements_len *= 2;
            statements = PyMem_Realloc(statements, sizeof(PyHP_PageCode_Cell) * statements_len);
            if (statements == NULL)
            {
                PyMem_Free(dst);
                PyErr_NoMemory();
                return 1;
            }
        }

        if (match.type == PyHP_StatementType_RAW)
        {
            st_pos = pos;
            st_len = match.end - match.start;
            memcpy(&(dst[pos]), &(src[match.start]), st_len);
            new_len = pos + st_len;
            pos = new_len;
        }
        else
        {
            st_pos = pos;
            st_len = PyHP_AlignCode(&(dst[pos]), src, match.start, match.end - match.start);
            if (st_len < 0)
            {
                return 1;
            }
            dst[st_pos + st_len] = 0; /* for python compiler */
        }

        if (PyHP_CompileCell(&(statements[statements_pos++]), PY_MAJOR_VERSION, match.type, dst, st_pos, st_len, optimize) != 0)
        {
            return 1;
        }
    }

    *p_slen = statements_pos;
    *p_statements = statements;
    *p_blen = new_len;
    *p_buffer = dst;

    return 0;
}

void PyHP_ReleaseCompile(PyHP_PageCode_Cell *p_statements, char *p_buffer)
{
    PyMem_Free(p_statements);
    PyMem_Free(p_buffer);
}

PyHP_PageCode *PyHP_AllocPageCode(Py_ssize_t slen, Py_ssize_t blen)
{
    return PyMem_Malloc(PyHP_PageCode_BASESIZE + PyHP_PageCode_EXTRASIZE(slen, blen));
}

void PyHP_FreePageCode(PyHP_PageCode *self)
{
    PyMem_Free(self);
}

void PyHP_ReleasePageCode(PyHP_PageCode *self)
{
    Py_ssize_t i;
    PyHP_PageCode_Cell *cell;

    Py_DECREF(self->name);

    i = PyHP_PageCode_SLEN(self);
    cell = PyHP_PageCode_STATEMENTS(self);
    while (i-- > 0)
    {
        switch (cell->type)
        {

            case PyHP_PageCodeType_EXEC:
            case PyHP_PageCodeType_EVAL:
                Py_DECREF(cell->value.code);
                break;
        }
        cell++;
    }
}


PyHP_PageCode_Object *PyHP_Compile_Func(PyObject *module, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"", "name", "optimize", NULL};
    PyObject *source;
    PyObject *name;
    int optimize;
    const char *src;
    Py_ssize_t len;
    Py_ssize_t slen;
    PyHP_PageCode_Cell *statements;
    Py_ssize_t blen;
    char *buffer;
    PyHP_PageCode_Object *self;

    optimize = -1;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "UU|$i", kw_list, &source, &name, &optimize))
    {
        return NULL;
    }

    src = PyHP_EncodeStringRO(source, &len);
    if (src == NULL)
    {
        return NULL;
    }

    if (PyHP_Compile(src, len, &slen, &statements, &blen, &buffer, optimize) != 0)
    {
        return NULL;
    }

    self = (PyHP_PageCode_Object *) (PyHP_PageCode_Type.tp_alloc(&PyHP_PageCode_Type, PyHP_PageCode_EXTRASIZE(slen, blen)));
    if (self == NULL)
    {
        PyHP_ReleaseCompile(statements, buffer);
        PyErr_NoMemory();
        return NULL;
    }

    PyHP_PageCode_SLEN(&(self->data)) = slen;
    PyHP_PageCode_BLEN(&(self->data)) = blen;

    memcpy(PyHP_PageCode_STATEMENTS(&(self->data)), statements, sizeof(PyHP_PageCode_Cell) * slen);
    memcpy((char *) (PyHP_PageCode_BUFFER(&(self->data))), buffer, sizeof(char) * blen);
    PyHP_ReleaseCompile(statements, buffer);

    Py_INCREF(name);
    self->data.name = name;

    return self;
}

static void PyHP_PageCode_Dealloc(PyHP_PageCode_Object *self)
{
    PyHP_ReleasePageCode(&self->data);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *PyHP_PageCode_Repr(PyHP_PageCode_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object '%U'>",
            Py_TYPE(self)->tp_name,
            self->data.name
    );
}

static PyHP_PageCodeIterator_Object *PyHP_PageCode_Iter(PyHP_PageCode_Object *self)
{
    PyHP_PageCodeIterator_Object *iterator;

    iterator = (PyHP_PageCodeIterator_Object *) (PyHP_PageCodeIterator_Type.tp_alloc(&PyHP_PageCodeIterator_Type, 0));
    if (iterator == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    Py_INCREF(self);
    iterator->data = self;
    iterator->pos = 0;

    return iterator;
}

PyTypeObject PyHP_PageCode_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.page_code",
        .tp_basicsize = PyHP_PageCode_BASESIZE + sizeof(PyObject),
        .tp_itemsize = 1,
        .tp_dealloc = (destructor) PyHP_PageCode_Dealloc,
        .tp_repr = (reprfunc) PyHP_PageCode_Repr,
        .tp_iter = (getiterfunc) PyHP_PageCode_Iter,
};


static void PyHP_PageCodeIterator_Dealloc(PyHP_PageCodeIterator_Object *self)
{
    Py_DECREF(self->data);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *PyHP_PageCodeIterator_Repr(PyHP_PageCodeIterator_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object of %R>",
            Py_TYPE(self)->tp_name,
            self->data
    );
}

static PyHP_PageCodeIterator_Object *PyHP_PageCodeIterator_Iter(PyHP_PageCodeIterator_Object *self)
{
    Py_INCREF(self);
    return self;
}

static PyObject *PyHP_PageCodeIterator_Next(PyHP_PageCodeIterator_Object *self)
{
    PyObject *tuple;
    PyObject *statement;
    PyHP_PageCode_Cell data;

    if (self->pos >= PyHP_PageCode_SLEN(&(self->data->data)))
    {
        return NULL;
    }


    data = PyHP_PageCode_STATEMENTS(&(self->data->data))[self->pos];

    if (data.type == PyHP_PageCodeType_TEXT)
    {
        statement = PyHP_DecodeString(PyHP_PageCode_BUFFER(&(self->data->data)), data.value.raw.start, data.value.raw.len);
        if (statement == NULL)
        {
            return NULL;
        }
    }
    else
    {
        statement = data.value.code;
        Py_INCREF(statement);
    }

    tuple = Py_BuildValue("iO", data.type, statement);
    Py_DECREF(statement);
    if (tuple == NULL)
    {
        return NULL;
    }
    self->pos++;
    return tuple;
}

PyTypeObject PyHP_PageCodeIterator_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.page_code_iterator",
        .tp_basicsize = sizeof(PyHP_PageCodeIterator_Object),
        .tp_dealloc = (destructor) PyHP_PageCodeIterator_Dealloc,
        .tp_repr = (reprfunc) PyHP_PageCodeIterator_Repr,
        .tp_iter = (getiterfunc) PyHP_PageCodeIterator_Iter,
        .tp_iternext = (iternextfunc) PyHP_PageCodeIterator_Next,
};


int PyHP_PageCodeArgConverter(PyObject *input, PyHP_PageCode_Object **dst)
{
    const char *src;
    Py_ssize_t len;

    if (Py_TYPE(input) != &PyHP_PageCode_Type)
    {
        if (!(PyUnicode_Check(input)))
        {
            PyErr_Format(
                    PyExc_TypeError,
                    "Can compile only strings"
            );
            return NULL;
        }

        src = PyHP_EncodeStringRO(input, &len);
        if (src == NULL)
        {
            return NULL;

        }

        icode.name = PyUnicode_FromString("<py3hp page>");
        if (icode.name == NULL)
        {
            return NULL;

        }

        code = &icode;
        if (PyHP_Compile(src, len, &(PyHP_PageCode_SLEN(code)), (PyHP_PageCode_Cell **) &(icode.statements), &(PyHP_PageCode_BLEN(code)), (char **) &(icode.buffer), -1) != 0)
        {
            Py_DECREF(icode.name);
            return NULL;

        }
    }
    else
    {
        Py_INCREF(input);
        *dst = (PyHP_PageCode_Object *) input;
    }

}