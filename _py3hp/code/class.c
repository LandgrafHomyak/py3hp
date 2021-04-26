#include <Python.h>
#include <structmember.h>

#include "class.h"
#include "parser.h"
#include "iterator.h"

static char anonymous_file[] = "<anonymous>";

static PyObject *anonymous_file_o = NULL;

static PyObject *compile_function = NULL;


static void Py3hpCode_Dealloc(Py3hpCode_Object *self)
{
    Py_ssize_t i;
    Py_ssize_t len;

    len = Py_SIZE(self);
    for (i = 0; i < len; i++)
    {
        switch (self->commands[i].type)
        {
            case Py3hpCode_Command_RAW:
                break;
            case Py3hpCode_Command_EVAL:
            case Py3hpCode_Command_EXEC:
                Py_DECREF(self->commands[i].pointer.code);
                break;
            default:
                break;
        }
    }
    PyMem_Free(self->buffer);
    Py_TYPE(self)->tp_free(self);
}


PyTypeObject Py3hpCodeBytes_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.py3hp_code_bytes",
        .tp_doc = "",
        .tp_basicsize = sizeof(Py3hpCode_Object) - sizeof(Py3hpCode_Command[1]),
        .tp_itemsize = sizeof(Py3hpCode_Command),
        .tp_new = NULL,
        .tp_dealloc = (destructor) Py3hpCode_Dealloc,
        .tp_iter = (getiterfunc) Py3hpCode_Iter,
};

PyTypeObject Py3hpCodeStr_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.py3hp_code_str",
        .tp_doc = "",
        .tp_base = &Py3hpCodeBytes_Type,
};


int Code_Class_Init(PyObject *module)
{
    if (PyType_Ready(&Py3hpCodeBytes_Type))
    {
        return -1;
    }

    if (PyType_Ready(&Py3hpCodeStr_Type))
    {
        return -1;
    }

    PyModule_AddObject(module, "py3hp_code_bytes", (PyObject *) &Py3hpCodeBytes_Type);
    PyModule_AddObject(module, "py3hp_code_str", (PyObject *) &Py3hpCodeStr_Type);

    return 0;
}

void Code_Class_Free(void)
{
}


