#include <Python.h>

#include "code.h"

static PyMethodDef module_functions[] = {
        {"compile", (PyCFunction) Py3hp_Compile_O, METH_VARARGS | METH_KEYWORDS, ""},
        {NULL}
};


static void module_free(void *any)
{
    Code_Free();
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

    if (Code_Init(module) == -1)
    {
        return NULL;
    }

    return module;
}
