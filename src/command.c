#include <Python.h>
#include <structmember.h>

#include <PyHP.h>

static void PyHP_Command_Dealloc(PyHP_Command_Object *self)
{
    Py_DECREF(self->data.value);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *PyHP_Command_Repr(PyHP_Command_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object; type=%s>",
            Py_TYPE(self)->tp_name,
            PyHP_CommandType_ToObject(self->data.type)->name
    );
}


static const PyHP_CommandType_Object *PyHP_Command_GetType(PyHP_Command_Object *self)
{
    return PyHP_CommandType_ToObject(self->data.type);
}


static PyGetSetDef PyHP_Command_GetSet[] = {
        {"type", (getter) PyHP_Command_GetType, NULL, ""},
        {NULL}
};

static PyMemberDef PyHP_Command_Members[] = {
        {"value", T_OBJECT, offsetof(PyHP_Command_Object, data.value), READONLY},
        {NULL}
};

PyTypeObject PyHP_Command_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "pyhp.compiler.command",
        .tp_basicsize = sizeof(PyHP_Command_Object),
        .tp_dealloc = (destructor) PyHP_Command_Dealloc,
        .tp_repr = (reprfunc) PyHP_Command_Repr,
        .tp_getset = PyHP_Command_GetSet,
        .tp_members = PyHP_Command_Members,
};

int PyHP_Command_Converter(PyHP_Command_Object *src, PyHP_Command *dst)
{
    if (Py_TYPE(src) != &PyHP_Command_Type)
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
