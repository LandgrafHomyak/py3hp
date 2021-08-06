#include <Python.h>
#include <structmember.h>
#include <PyHP.h>


static PyObject *PyHP_PrepareResult_GetValue(PyHP_PrepareResult_Object *self)
{
    switch (self->data.type)
    {
        case PyHP_StatementType_RAW:
            Py_INCREF(self->data.string);
            return self->data.string;
        case PyHP_StatementType_INLINE:
        case PyHP_StatementType_BLOCK:
            Py_INCREF(self->raw_string);
            return self->raw_string;
    }
}

static void PyHP_PrepareResult_Dealloc(PyHP_PrepareResult_Object *self)
{
    PyObject *s;
    s = PyHP_PrepareResult_GetValue(self);
    Py_DECREF(s);
    Py_DECREF(s);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *PyHP_PrepareResult_Repr(PyHP_PrepareResult_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object; type=%s>",
            Py_TYPE(self)->tp_name,
            PyHP_StatementType_ToObject(self->data.type)->name
    );
}


static const PyHP_StatementType_Object *PyHP_PrepareResult_GetType(PyHP_PrepareResult_Object *self)
{
    return PyHP_StatementType_ToObject(self->data.type);
}


static PyGetSetDef PyHP_PrepareResult_GetSet[] = {
        {"type", (getter) PyHP_PrepareResult_GetType, NULL, ""},
        {"value", (getter) PyHP_PrepareResult_GetValue, NULL, ""},
        {NULL}
};

PyTypeObject PyHP_PrepareResult_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "pyhp.parser.prepare_result",
        .tp_basicsize = sizeof(PyHP_ParserMatch_Object),
        .tp_dealloc = (destructor) PyHP_PrepareResult_Dealloc,
        .tp_repr = (reprfunc) PyHP_PrepareResult_Repr,
        .tp_getset = PyHP_PrepareResult_GetSet,
};

int PyHP_PrepareResult_Converter(PyHP_PrepareResult_Object *src, PyHP_PrepareResult *dst)
{
    if (Py_TYPE(src) != &PyHP_PrepareResult_Type)
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
