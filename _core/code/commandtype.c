#include <Python.h>
#include <structmember.h>

#include "commandtype.h"

typedef struct
{
    PyObject_HEAD
    Py3hpCode_Command_Type id;
    char *name;
} Py3hpCode_CommandType_Object;

static PyMemberDef Py3hpCode_CommandType_Members[] = {
        {"id",   T_INT,    offsetof(Py3hpCode_CommandType_Object, id),   READONLY, ""},
        {"name", T_STRING, offsetof(Py3hpCode_CommandType_Object, name), READONLY, ""},
        {NULL}
};

static PyObject *Py3hpCode_CommandType_Int(Py3hpCode_CommandType_Object *self)
{
    return PyLong_FromLong(self->id);
}

static PyNumberMethods Py3hpCode_CommandType_Numbers = {
        .nb_int = (unaryfunc) Py3hpCode_CommandType_Int,
        .nb_index = (unaryfunc) Py3hpCode_CommandType_Int,
};

static PyObject *Py3hpCode_CommandType_Str(Py3hpCode_CommandType_Object *self)
{
    return PyUnicode_FromString(self->name);
}

static PyObject *Py3hpCode_CommandType_Repr(Py3hpCode_CommandType_Object *self)
{
    return PyUnicode_FromFormat("<py3hp_code command type '%s' id=%d>", self->name, self->id);
}

static void Py3hpCode_CommandType_Dealloc(Py3hpCode_CommandType_Object *self)
{

}

static PyTypeObject Py3hpCode_CommandType_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.code_commandtype",
        .tp_doc = "",
        .tp_basicsize = sizeof(Py3hpCode_CommandType_Object),
        .tp_itemsize = 0,
        .tp_members = Py3hpCode_CommandType_Members,
        .tp_as_number = &Py3hpCode_CommandType_Numbers,
        .tp_str = (reprfunc) Py3hpCode_CommandType_Str,
        .tp_repr = (reprfunc) Py3hpCode_CommandType_Repr,
        .tp_dealloc = (destructor) Py3hpCode_CommandType_Dealloc
};

static Py3hpCode_CommandType_Object Py3hpCode_CommandType_Instances[] = {
        {
                PyObject_HEAD_INIT(&Py3hpCode_CommandType_Type)
                Py3hpCode_Command_RAW,
                "raw"
        },
        {
                PyObject_HEAD_INIT(&Py3hpCode_CommandType_Type)
                Py3hpCode_Command_EVAL,
                "eval"
        },
        {
                PyObject_HEAD_INIT(&Py3hpCode_CommandType_Type)
                Py3hpCode_Command_EXEC,
                "exec"
        },
};

PyObject *Py3hpCode_CommandType_Get(Py3hpCode_Command_Type id)
{
    return (PyObject *) &(Py3hpCode_CommandType_Instances[id]);
}

int Code_CommandType_Init(PyObject *module)
{
    if (PyType_Ready(&Py3hpCode_CommandType_Type))
    {
        return -1;
    }

    if (PyModule_AddObject(module, "code_COMMAND_RAW", (PyObject *) &(Py3hpCode_CommandType_Instances[Py3hpCode_Command_RAW])) == -1)
    {
        return -1;
    }
    if (PyModule_AddObject(module, "code_COMMAND_EVAL", (PyObject *) &(Py3hpCode_CommandType_Instances[Py3hpCode_Command_EVAL])) == -1)
    {
        return -1;
    }
    if (PyModule_AddObject(module, "code_COMMAND_EXEC", (PyObject *) &(Py3hpCode_CommandType_Instances[Py3hpCode_Command_EXEC])) == -1)
    {
        return -1;
    }

    return 0;
}

void Code_CommandType_Free(void)
{
}
