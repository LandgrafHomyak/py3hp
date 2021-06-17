#include <string.h>

#include <Python.h>
#include "structmember.h"

#include <PyHP/api.h>
#include <PyHP/core.h>

PyObject *PyHP_Core_AlignCode_Func(PyObject *module, PyObject *string)
{
    const char *src;
    char *dst;
    Py_ssize_t len;
    PyObject *new;

    if (!(PyUnicode_Check(string)))
    {
        PyErr_Format(PyExc_TypeError, "source must be str, not %s", Py_TYPE(string)->tp_name);
    }

    src = PyHP_API_EncodeStringRO(string, &len);
    if (src == NULL)
    {
        return NULL;
    }

    dst = PyMem_Malloc(len);
    if (dst == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    len = PyHP_API_AlignCode(dst, src, 0, len);
    if (len == -1)
    {
        PyMem_Free(dst);
        return NULL;
    }

    new = PyHP_API_DecodeString(dst, 0, len);
    PyMem_Free(dst);
    if (new == NULL)
    {
        return NULL;
    }

    return new;
}

PyHP_Core_ParserIterator_Object *PyHP_Core_Parser_Func(PyObject *module, PyObject *string)
{
    PyHP_Core_ParserIterator_Object *self;
    const char *src;
    Py_ssize_t len;

    if (!(PyUnicode_Check(string)))
    {
        PyErr_Format(PyExc_TypeError, "source must be str, not %s", Py_TYPE(string)->tp_name);
    }

    src = PyHP_API_EncodeStringRO(string, &len);
    if (src == NULL)
    {
        return NULL;
    }

    self = (PyHP_Core_ParserIterator_Object *) (PyHP_Core_ParserIterator_Type.tp_alloc(&PyHP_Core_ParserIterator_Type, len));
    if (self == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    memcpy(self->buffer, src, len);
    PyHP_API_Parser_Init(self->buffer, len, &self->state);

    return self;
}

static PyHP_Core_ParserIterator_Object *PyHP_Core_ParserIterator_Iter(PyHP_Core_ParserIterator_Object *self)
{
    Py_INCREF(self);
    return self;
}

static PyHP_Core_ParserMatch_Object *PyHP_Core_ParserIterator_Next(PyHP_Core_ParserIterator_Object *self)
{
    PyHP_API_ParserMatch match;
    PyHP_Core_ParserMatch_Object *object;

    match = PyHP_API_Parser_Next(self->buffer, Py_SIZE(self), &(self->state));

    if (match.type == PyHP_API_StatementType_NONE)
    {
        return NULL;
    }

    object = (PyHP_Core_ParserMatch_Object *) (PyHP_Core_ParserMatch_Type.tp_alloc(&PyHP_Core_ParserMatch_Type, 0));
    if (object == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    object->meta = match;
    object->value = PyHP_API_DecodeString(self->buffer, match.start, match.end - match.start);
    if (object->value == NULL)
    {
        Py_DECREF(object);
        return NULL;
    }

    return object;
}

static void PyHP_Core_ParserIterator_Dealloc(PyHP_Core_ParserIterator_Object *self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject *PyHP_Core_ParserIterator_GetSource(PyHP_Core_ParserIterator_Object *self)
{
    return PyHP_API_DecodeString(self->buffer, 0, Py_SIZE(self));
}

PyGetSetDef PyHP_Core_ParserIterator_GetSet[] = {
        {"_source", (getter) PyHP_Core_ParserIterator_GetSource, NULL, ""},
        {NULL}
};

PyTypeObject PyHP_Core_ParserIterator_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.parser_iterator",
        .tp_basicsize = sizeof(PyHP_Core_ParserIterator_Object) - sizeof(char[1]),
        .tp_itemsize = sizeof(char),
        .tp_dealloc = (destructor) PyHP_Core_ParserIterator_Dealloc,
        .tp_iter = (getiterfunc) PyHP_Core_ParserIterator_Iter,
        .tp_iternext = (iternextfunc) PyHP_Core_ParserIterator_Next,
        .tp_getset = PyHP_Core_ParserIterator_GetSet,
};


static void PyHP_Core_ParserMatch_Dealloc(PyHP_Core_ParserMatch_Object *self)
{
    Py_XDECREF(self->value);
    Py_TYPE(self)->tp_free(self);
}

static PyMemberDef PyHP_Core_ParserMatch_Members[] = {
        {"value", T_OBJECT,   offsetof(PyHP_Core_ParserMatch_Object, value),      READONLY},
        {"start", T_PYSSIZET, offsetof(PyHP_Core_ParserMatch_Object, meta.start), READONLY},
        {"end",   T_PYSSIZET, offsetof(PyHP_Core_ParserMatch_Object, meta.end),   READONLY},
        {NULL}
};

static PyObject *PyHP_Core_ParserMatch_GetType(PyHP_Core_ParserMatch_Object *self)
{
    return PyLong_FromLong(self->meta.type);
}

static PyGetSetDef PyHP_Core_ParserMatch_GetSet[] = {
        {"type", (getter) PyHP_Core_ParserMatch_GetType, NULL, ""},
        {NULL}
};

static PyObject *PyHP_Core_ParserMatch_Repr(PyHP_Core_ParserMatch_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object; span=(%zd, %zd) type=%d>",
            Py_TYPE(self)->tp_name,
            self->meta.start,
            self->meta.end,
            self->meta.type
    );
}

static PyObject *PyHP_Core_ParserMatch_Str(PyHP_Core_ParserMatch_Object *self)
{
    Py_INCREF(self);
    return self->value;
}

PyTypeObject PyHP_Core_ParserMatch_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.parser_match",
        .tp_basicsize = sizeof(PyHP_Core_ParserMatch_Object),
        .tp_dealloc = (destructor) PyHP_Core_ParserMatch_Dealloc,
        .tp_repr = (reprfunc) PyHP_Core_ParserMatch_Repr,
        .tp_members = PyHP_Core_ParserMatch_Members,
        .tp_getset = PyHP_Core_ParserMatch_GetSet,
        /* .tp_str = (reprfunc) PyHP_Core_ParserMatch_Str, */
};