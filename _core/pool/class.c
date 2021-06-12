#include <Python.h>

#include "class.h"
#include "subinterpeter.h"

void Py3hpPool_Dealloc(Py3hpPool_Object *self)
{
    if (self->array != NULL)
    {
        PyMem_Free(self->array);
    }
    Py_TYPE(self)->tp_free(self);
}

PyObject *Py3hpPool_Str(Py3hpPool_Object *self)
{
    return PyUnicode_FromFormat("<>");
}

PyTypeObject Py3hpPool_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.pool",
        .tp_doc = "",
        .tp_basicsize = sizeof(Py3hpPool_Object),
        .tp_itemsize = 0,
        .tp_new = NULL,
        .tp_dealloc = (destructor) Py3hpPool_Dealloc,
        .tp_str = (reprfunc) Py3hpPool_Str
};

PyTypeObject Py3hpDefaultPool_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.default_pool",
        .tp_doc = "",
        .tp_base = &Py3hpPool_Type
};

Py3hpPool_Object *Py3hpPool_FromMaster(PyThreadState *master)
{
    Py3hpPool_Object *self;

    self = (Py3hpPool_Object *) Py3hpPool_Type.tp_alloc(&Py3hpPool_Type, 0);
    if (self == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    self->master = master;
    self->len = 0;
    self->array = NULL;
    self->flags.si_pre_init = 0;

    return self;
}

void Py3hpPool_MakeDefault(Py3hpPool_Object *o)
{
    Py_TYPE(o) = &Py3hpDefaultPool_Type;
}


int Pool_Class_Init(PyObject *module)
{
    if (PyType_Ready(&Py3hpPool_Type))
    {
        return -1;
    }
    if (PyType_Ready(&Py3hpDefaultPool_Type))
    {
        return -1;
    }


    PyModule_AddObject(module, "pool", (PyObject *) &Py3hpPool_Type);

    return 0;
}

void Pool_Class_Free(void)
{
}