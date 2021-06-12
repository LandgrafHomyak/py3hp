#include <Python.h>
/*
#include "code.h"
#include "pool.h"*/

#include "debug.h"
#include "streams.h"

PyObject *qhdecjwc(PyObject *module)
{
    return (PyObject *) Py3hpCore_OutStream_Create(&Py3hpCore_PageStdout_Type);
}

static PyMethodDef module_functions[] = {
        {"ps", (PyCFunction)qhdecjwc, METH_NOARGS, ""},
        /* {"compile",          (PyCFunction) Py3hp_Compile_O,        METH_VARARGS | METH_KEYWORDS, ""},
         {"get_default_pool", (PyCFunction) Py3hp_GetDefaultPool_O, METH_NOARGS, ""},
         {"sub",              (PyCFunction) Py3hp_Sub,              METH_NOARGS, ""},*/
        {NULL}
};


static void module_free(void *any)
{
//    Code_Free();
}

static PyModuleDef module_def = {
        PyModuleDef_HEAD_INIT,
        .m_name="py3hp.core",
        .m_doc = "",
        .m_size = -1,
        .m_methods = module_functions,
        .m_free = (freefunc) module_free
};

PyMODINIT_FUNC PyInit_core(void)
{
    PyObject *module;

    module = PyModule_Create(&module_def);
    if (module == NULL)
    {
        return NULL;
    }

    /* if (Code_Init(module) == -1)
     {
         return NULL;
     }

     if (Pool_Init(module) == -1)
     {
         return NULL;
     }*/

    if (
            PyType_Ready(&Py3hpCore_PageStderr_Type) ||
            PyType_Ready(&Py3hpCore_PageStdout_Type)
            )
    {
        return NULL;

    }
    return module;

}
