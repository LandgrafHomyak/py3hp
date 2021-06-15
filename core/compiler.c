#include <Python.h>
#include "py3hp.h"

#include "compiler.h"
#include "encoding.h"
#include "parser.h"


PY3HP_LOW_API int Py3hp_Core_CompileCell(Py3hp_Core_PageCode_Cell *dst, int py_version, Py3hp_Core_StatementType type, const char *source, Py_ssize_t start, Py_ssize_t len, int optimize)
{
    switch (type)
    {
        case Py3hp_Core_StatementType_RAW:
            dst->type = Py3hp_Core_PageCodeType_TEXT;
            dst->value.raw.start = start;
            dst->value.raw.len = len;
            return 0;
        case Py3hp_Core_StatementType_BLOCK:
            if (py_version == 3)
            {
                return Py3hp_Core_CompileCell(dst, py_version, Py3hp_Core_StatementType_BLOCK3, source, start, len, optimize);
            }
        case Py3hp_Core_StatementType_INLINE:
            if (py_version == 3)
            {
                return Py3hp_Core_CompileCell(dst, py_version, Py3hp_Core_StatementType_INLINE3, source, start, len, optimize);
            }
        case Py3hp_Core_StatementType_BLOCK2:
        case Py3hp_Core_StatementType_BLOCK1:
        case Py3hp_Core_StatementType_INLINE2:
        case Py3hp_Core_StatementType_INLINE1:
            PyErr_Format(PyExc_NotImplementedError, "support of lower python versions not implemented yet");
            return 1;
        case Py3hp_Core_StatementType_INLINE3:
#if PY_VERSION_HEX >= 0x03020000
            dst->value.code = Py_CompileStringExFlags(source + start, "<py3hp page inline insertion>", Py_eval_input, NULL, optimize);
#else
            dst->value.code = Py_CompileString(source + start, "<py3hp page inline insertion>", Py_eval_input);
#endif
            if (dst->value.code == NULL)
            {
                return 1;
            }
            dst->type = Py3hp_Core_PageCodeType_EVAL;
            return 0;
        case Py3hp_Core_StatementType_BLOCK3:
#if PY_VERSION_HEX >= 0x03020000
            dst->value.code = Py_CompileStringExFlags(source + start, "<py3hp page block insertion>", Py_file_input, NULL, optimize);
#else
            dst->value.code = Py_CompileString(source + start, "<py3hp page block insertion>", Py_file_input);
#endif
            if (dst->value.code == NULL)
            {
                return 1;
            }
            dst->type = Py3hp_Core_PageCodeType_EXEC;
            return 0;
        default:
            PyErr_BadInternalCall();
            return 1;
    }
}

PY3HP_LOW_API int Py3hp_Core_Compile(const char *src, Py_ssize_t len, Py_ssize_t *p_slen, Py3hp_Core_PageCode_Cell **p_statements, Py_ssize_t *p_blen, char **p_buffer, int optimize)
{
    Py3hp_Core_ParserIteratorState iterator;
    Py3hp_Core_ParserMatch match;
    char *dst;
    Py_ssize_t new_len;
    Py_ssize_t pos;
    Py_ssize_t st_pos;
    Py_ssize_t st_len;
    Py3hp_Core_PageCode_Cell *statements;
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
    statements = PyMem_Malloc(sizeof(Py3hp_Core_PageCode_Cell) * statements_len);
    if (statements == NULL)
    {
        PyMem_Free(dst);
        PyErr_NoMemory();
        return 1;
    }


    Py3hp_Core_Parser_Init(src, len, &iterator);
    while ((match = Py3hp_Core_Parser_Next(src, len, &iterator)).type != Py3hp_Core_StatementType_NONE)
    {
        if (statements_pos >= statements_len)
        {
            statements_len *= 2;
            statements = PyMem_Realloc(statements, sizeof(Py3hp_Core_PageCode_Cell) * statements_len);
            if (statements == NULL)
            {
                PyMem_Free(dst);
                PyErr_NoMemory();
                return 1;
            }
        }

        if (match.type == Py3hp_Core_StatementType_RAW)
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
            st_len = Py3hp_Core_AlignCode(&(dst[pos]), src, match.start, match.end - match.start);
            if (st_len < 0)
            {
                return 1;
            }
            dst[st_pos + st_len] = 0; /* for python compiler */
        }

        if (Py3hp_Core_CompileCell(&(statements[statements_pos++]), PY_MAJOR_VERSION, match.type, dst, st_pos, st_len, optimize) != 0)
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

PY3HP_LOW_API void Py3hp_Core_ReleaseCompile(Py3hp_Core_PageCode_Cell *p_statements, char *p_buffer)
{
    PyMem_Free(p_statements);
    PyMem_Free(p_buffer);
}

PY3HP_LOW_API Py3hp_Core_PageCode *Py3hp_Core_AllocPageCode(Py_ssize_t slen, Py_ssize_t blen)
{
    return PyMem_Malloc(Py3hp_Core_PageCode_BASESIZE + Py3hp_Core_PageCode_EXTRASIZE(slen, blen));
}

PY3HP_LOW_API void Py3hp_Core_FreePageCode(Py3hp_Core_PageCode *self)
{
    PyMem_Free(self);
}

PY3HP_LOW_API void Py3hp_Core_ReleasePageCode(Py3hp_Core_PageCode *self)
{
    Py_ssize_t i;
    Py3hp_Core_PageCode_Cell *cell;

    Py_DECREF(self->name);

    i = Py3hp_Core_PageCode_SLEN(self);
    cell = Py3hp_Core_PageCode_STATEMENTS(self);
    while (i-- > 0)
    {
        switch (cell->type)
        {

            case Py3hp_Core_PageCodeType_EXEC:
            case Py3hp_Core_PageCodeType_EVAL:
                Py_DECREF(cell->value.code);
                break;
        }
        cell++;
    }
}

PY3HP_HIGH_API Py3hp_Core_PageCode_Object *Py3hp_Core_Compile_Func(PyObject *module, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"", "name", "optimize", NULL};
    PyObject *source;
    PyObject *name;
    int optimize;
    const char *src;
    Py_ssize_t len;
    Py_ssize_t slen;
    Py3hp_Core_PageCode_Cell *statements;
    Py_ssize_t blen;
    char *buffer;
    Py3hp_Core_PageCode_Object *self;

    optimize = -1;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "UU|$i", kw_list, &source, &name, &optimize))
    {
        return NULL;
    }

    src = Py3hp_Core_EncodeStringRO(source, &len);
    if (src == NULL)
    {
        return NULL;
    }

    if (Py3hp_Core_Compile(src, len, &slen, &statements, &blen, &buffer, optimize) != 0)
    {
        return NULL;
    }

    self = (Py3hp_Core_PageCode_Object *) (Py3hp_Core_PageCode_Type.tp_alloc(&Py3hp_Core_PageCode_Type, Py3hp_Core_PageCode_EXTRASIZE(slen, blen)));
    if (self == NULL)
    {
        Py3hp_Core_ReleaseCompile(statements, buffer);
        PyErr_NoMemory();
        return NULL;
    }

    Py3hp_Core_PageCode_SLEN(&(self->data)) = slen;
    Py3hp_Core_PageCode_BLEN(&(self->data)) = blen;

    memcpy(Py3hp_Core_PageCode_STATEMENTS(&(self->data)), statements, sizeof(Py3hp_Core_PageCode_Cell) * slen);
    memcpy((char *) (Py3hp_Core_PageCode_BUFFER(&(self->data))), buffer, sizeof(char) * blen);
    Py3hp_Core_ReleaseCompile(statements, buffer);

    Py_INCREF(name);
    self->data.name = name;

    return self;
}

static void Py3hp_Core_PageCode_Dealloc(Py3hp_Core_PageCode_Object *self)
{
    Py3hp_Core_ReleasePageCode(&self->data);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *Py3hp_Core_PageCode_Repr(Py3hp_Core_PageCode_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object '%U'>",
            Py_TYPE(self)->tp_name,
            self->data.name
    );
}

static Py3hp_Core_PageCodeIterator_Object *Py3hp_Core_PageCode_Iter(Py3hp_Core_PageCode_Object *self)
{
    Py3hp_Core_PageCodeIterator_Object *iterator;

    iterator = (Py3hp_Core_PageCodeIterator_Object *) (Py3hp_Core_PageCodeIterator_Type.tp_alloc(&Py3hp_Core_PageCodeIterator_Type, 0));
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

PyTypeObject Py3hp_Core_PageCode_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.page_code",
        .tp_basicsize = Py3hp_Core_PageCode_BASESIZE,
        .tp_itemsize = 1,
        .tp_dealloc = (destructor) Py3hp_Core_PageCode_Dealloc,
        .tp_repr = (reprfunc) Py3hp_Core_PageCode_Repr,
        .tp_iter = (getiterfunc) Py3hp_Core_PageCode_Iter,
};


static void Py3hp_Core_PageCodeIterator_Dealloc(Py3hp_Core_PageCodeIterator_Object *self)
{
    Py_DECREF(self->data);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *Py3hp_Core_PageCodeIterator_Repr(Py3hp_Core_PageCodeIterator_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object of %R>",
            Py_TYPE(self)->tp_name,
            self->data
    );
}

static Py3hp_Core_PageCodeIterator_Object *Py3hp_Core_PageCodeIterator_Iter(Py3hp_Core_PageCodeIterator_Object *self)
{
    Py_INCREF(self);
    return self;
}

static PyObject *Py3hp_Core_PageCodeIterator_Next(Py3hp_Core_PageCodeIterator_Object *self)
{
    PyObject *tuple;
    PyObject *statement;
    Py3hp_Core_PageCode_Cell data;

    if (self->pos >= Py3hp_Core_PageCode_SLEN(&(self->data->data)))
    {
        return NULL;
    }


    data = Py3hp_Core_PageCode_STATEMENTS(&(self->data->data))[self->pos];

    if (data.type == Py3hp_Core_PageCodeType_TEXT)
    {
        statement = Py3hp_Core_DecodeString(Py3hp_Core_PageCode_BUFFER(&(self->data->data)), data.value.raw.start, data.value.raw.len);
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

PyTypeObject Py3hp_Core_PageCodeIterator_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.page_code_iterator",
        .tp_basicsize = sizeof(Py3hp_Core_PageCodeIterator_Object),
        .tp_dealloc = (destructor) Py3hp_Core_PageCodeIterator_Dealloc,
        .tp_repr = (reprfunc) Py3hp_Core_PageCodeIterator_Repr,
        .tp_iter = (getiterfunc) Py3hp_Core_PageCodeIterator_Iter,
        .tp_iternext = (iternextfunc) Py3hp_Core_PageCodeIterator_Next,
};
