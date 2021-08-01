#include <Python.h>
#include <structmember.h>

#include <PyHP.h>

static void PyHP_StatementType_Dealloc(PyHP_StatementType_Object *self)
{}

static PyObject *PyHP_StatementType_Repr(PyHP_StatementType_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s %s>",
            Py_TYPE(self)->tp_name,
            self->name
    );
}

static PyObject *PyHP_StatementType_Str(PyHP_StatementType_Object *self)
{
    return PyUnicode_FromString(self->name);
}

static PyObject *PyHP_StatementType_Int(PyHP_StatementType_Object *self)
{
    return PyLong_FromLongLong((long long) (self->value));
}

static PyNumberMethods PyHP_StatementType_AsNumber = {
        .nb_int = (unaryfunc) PyHP_StatementType_Int
};

PyTypeObject PyHP_StatementType_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "pyhp.parser.statement_type",
        .tp_basicsize = sizeof(PyHP_StatementType_Object),
        .tp_dealloc = (destructor) PyHP_StatementType_Dealloc,
        .tp_str = (reprfunc) PyHP_StatementType_Str,
        .tp_repr = (reprfunc) PyHP_StatementType_Repr,
        .tp_as_number = &PyHP_StatementType_AsNumber
};

#define INIT(NAME)                                                                                                           \
static PyHP_StatementType_Object PyHP_StatementTypeObject_ ## NAME ## _ = {                                                  \
    PyObject_HEAD_INIT(&PyHP_StatementType_Type)                                                                             \
    # NAME,                                                                                                                  \
    PyHP_StatementType_ ## NAME                                                                                              \
};                                                                                                                           \
const PyHP_StatementType_Object *const PyHP_StatementTypeObject_ ## NAME = &PyHP_StatementTypeObject_ ## NAME ## _;

INIT(NONE)
INIT(RAW)
INIT(INLINE)
INIT(BLOCK)

#undef INIT

const PyHP_StatementType_Object *PyHP_StatementType_ToObject(PyHP_StatementType e)
{
#define CASE(NAME) case PyHP_StatementType_ ## NAME: return PyHP_StatementTypeObject_ ## NAME;
    switch (e)
    {
        CASE(NONE)
        CASE(RAW)
        CASE(INLINE)
        CASE(BLOCK)
    }
#undef CASE
}

