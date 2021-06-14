#include <Python.h>
#include "py3hp.h"

#include "compiler.h"
#include "encoding.h"
#include "parser.h"


PY3HP_LOW_API int Py3hp_Core_Compile(Py3hp_Core_PageCode_Cell *dst, int py_version, Py3hp_Core_StatementType type, const char *source, Py_ssize_t start, Py_ssize_t len, int optimize)
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
                return Py3hp_Core_Compile(dst, py_version, Py3hp_Core_StatementType_BLOCK3, source, start, len, optimize);
            }
        case Py3hp_Core_StatementType_INLINE:
            if (py_version == 3)
            {
                return Py3hp_Core_Compile(dst, py_version, Py3hp_Core_StatementType_INLINE3, source, start, len, optimize);
            }
        case Py3hp_Core_StatementType_BLOCK2:
        case Py3hp_Core_StatementType_BLOCK1:
        case Py3hp_Core_StatementType_INLINE2:
        case Py3hp_Core_StatementType_INLINE1:
            PyErr_Format(PyExc_NotImplementedError, "support of lower python versions not implemented yet");
            return 1;
        case Py3hp_Core_StatementType_INLINE3:
#if PY_VERSION_HEX >= 0x03020000
            dst->value.code = Py_CompileStringExFlags(source + start, "<py3hp page inline insertion>", Py_single_input, NULL, optimize);
#else
            dst->value.code = Py_CompileString(source + start, "<py3hp page inline insertion>", Py_single_input);
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

PY3HP_HIGH_API Py3hp_Core_PageCode_Object *Py3hp_Core_Compile_Func(PyObject *module, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"", "name", "optimize", NULL};
    PyObject *source;
    PyObject *name;
    int optimize;
    const char *src;
    Py_ssize_t len;
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

    new_len = 0;
    pos = 0;
    dst = PyMem_Malloc(len);
    if (dst == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    statements_len = 8;
    statements_pos = 0;
    statements = PyMem_Malloc(sizeof(Py3hp_Core_PageCode_Cell) * statements_len);
    if (statements == NULL)
    {
        PyMem_Free(dst);
        PyErr_NoMemory();
        return NULL;
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
                return NULL;
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
                return NULL;
            }
            dst[st_pos + st_len] = 0; /* for python compiler */
        }

        if (Py3hp_Core_Compile(&(statements[statements_pos++]), PY_MAJOR_VERSION, match.type, dst, st_pos, st_len, optimize) != 0)
        {
            return NULL;
        }
    }

    self = (Py3hp_Core_PageCode_Object *) (Py3hp_Core_PageCode_Type.tp_alloc(&Py3hp_Core_PageCode_Type, sizeof(char) * new_len + sizeof(Py3hp_Core_PageCode_Cell) * statements_pos));
    if (self == NULL)
    {
        PyMem_Free(statements);
        PyMem_Free(dst);
        PyErr_NoMemory();
        return NULL;
    }

    Py3hp_Core_PageCode_BLEN(self) = new_len;
    Py3hp_Core_PageCode_SLEN(self) = statements_pos;

    memcpy((char *) (Py3hp_Core_PageCode_BUFFER(self)), dst, sizeof(char) * new_len);
    PyMem_Free(dst);

    memcpy(Py3hp_Core_PageCode_STATEMENTS(self), statements, sizeof(Py3hp_Core_PageCode_Cell) * statements_pos);
    PyMem_Free(statements);

    Py_INCREF(name);
    self->name = name;

    return self;
}

static void Py3hp_Core_PageCode_Dealloc(Py3hp_Core_PageCode_Object *self)
{
    Py_DECREF(self->name);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *Py3hp_Core_PageCode_Repr(Py3hp_Core_PageCode_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object '%U'>",
            Py_TYPE(self)->tp_name,
            self->name
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
        .tp_basicsize = sizeof(Py3hp_Core_PageCode_Object) - sizeof(Py3hp_Core_PageCode_Cell[1]) - sizeof(const char[1]),
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

    if (self->pos >= Py3hp_Core_PageCode_SLEN(self->data))
    {
        return NULL;
    }


    data = Py3hp_Core_PageCode_STATEMENTS(self->data)[self->pos];

    if (data.type == Py3hp_Core_PageCodeType_TEXT)
    {
        statement = Py3hp_Core_DecodeString(Py3hp_Core_PageCode_BUFFER(self->data), data.value.raw.start, data.value.raw.len);
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
