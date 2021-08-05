#include <Python.h>
#include <structmember.h>

#include <PyHP.h>

static void PyHP_CommandType_Dealloc(PyHP_CommandType_Object *self)
{}

static PyObject *PyHP_CommandType_Repr(PyHP_CommandType_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s %s>",
            Py_TYPE(self)->tp_name,
            self->name
    );
}

static PyObject *PyHP_CommandType_Str(PyHP_CommandType_Object *self)
{
    return PyUnicode_FromString(self->name);
}

static PyObject *PyHP_CommandType_Int(PyHP_CommandType_Object *self)
{
    return PyLong_FromLongLong((long long) (self->value));
}

static PyNumberMethods PyHP_CommandType_AsNumber = {
        .nb_int = (unaryfunc) PyHP_CommandType_Int
};

PyTypeObject PyHP_CommandType_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "pyhp.compiler.command_type",
        .tp_basicsize = sizeof(PyHP_CommandType_Object),
        .tp_dealloc = (destructor) PyHP_CommandType_Dealloc,
        .tp_str = (reprfunc) PyHP_CommandType_Str,
        .tp_repr = (reprfunc) PyHP_CommandType_Repr,
        .tp_as_number = &PyHP_CommandType_AsNumber
};

#define INIT(NAME)                                                                                                           \
static PyHP_CommandType_Object PyHP_CommandTypeObject_ ## NAME ## _ = {                                                  \
    PyObject_HEAD_INIT(&PyHP_CommandType_Type)                                                                             \
    # NAME,                                                                                                                  \
    PyHP_CommandType_ ## NAME                                                                                              \
};                                                                                                                           \
const PyHP_CommandType_Object *const PyHP_CommandTypeObject_ ## NAME = &PyHP_CommandTypeObject_ ## NAME ## _;

INIT(NONE)
INIT(TEXT)
INIT(EVAL)
INIT(EXEC)

#undef INIT

const PyHP_CommandType_Object *PyHP_CommandType_ToObject(PyHP_CommandType e)
{
#define CASE(NAME) case PyHP_CommandType_ ## NAME: return PyHP_CommandTypeObject_ ## NAME;
    switch (e)
    {
        CASE(NONE)
        CASE(TEXT)
        CASE(EVAL)
        CASE(EXEC)
    }
#undef CASE
}

