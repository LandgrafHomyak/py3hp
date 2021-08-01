#include <Python.h>
#include <structmember.h>

#include <PyHP.h>


static void PyHP_ParserMatch_Dealloc(PyHP_ParserMatch_Object *self)
{
    Py_XDECREF(self->value);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *PyHP_ParserMatch_Repr(PyHP_ParserMatch_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object; span=(%zd, %zd) type=%s>",
            Py_TYPE(self)->tp_name,
            self->data.start,
            self->data.end,
            PyHP_StatementType_ToObject(self->data.type)->name
    );
}


static PyMemberDef PyHP_ParserMatch_Members[] = {
        {"value", T_OBJECT,   offsetof(PyHP_ParserMatch_Object, value),      READONLY},
        {"start", T_PYSSIZET, offsetof(PyHP_ParserMatch_Object, data.start), READONLY},
        {"end",   T_PYSSIZET, offsetof(PyHP_ParserMatch_Object, data.end),   READONLY},
        {NULL}
};

static const PyHP_StatementType_Object *PyHP_ParserMatch_GetType(PyHP_ParserMatch_Object *self)
{
    return PyHP_StatementType_ToObject(self->data.type);
}

static PyGetSetDef PyHP_ParserMatch_GetSet[] = {
        {"type", (getter) PyHP_ParserMatch_GetType, NULL, ""},
        {NULL}
};

PyTypeObject PyHP_ParserMatch_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "pyhp.parser.parser_match",
        .tp_basicsize = sizeof(PyHP_ParserMatch_Object),
        .tp_dealloc = (destructor) PyHP_ParserMatch_Dealloc,
        .tp_repr = (reprfunc) PyHP_ParserMatch_Repr,
        .tp_members = PyHP_ParserMatch_Members,
        .tp_getset = PyHP_ParserMatch_GetSet,
};

int PyHP_ParserMatch_Converter(PyHP_ParserMatch_Object *src, PyHP_ParserMatch *dst)
{
    if (Py_TYPE(src) != &PyHP_ParserMatch_Type)
    {
        PyErr_Format(
                PyExc_TypeError,
                "Can't extract info from '%R' object",
                Py_TYPE(src)
        );
        return 1;
    }

    *dst = src->data;
    return 0;
}
