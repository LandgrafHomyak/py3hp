#include <Python.h>

#include "py3hp.h"
#include "parser.h"

static PyMethodDef module_functions[] = {
#if PY_VERSION_HEX >= 0x03000000
        {"parse", (PyCFunction) Py3hp_Core_Parser_Func, METH_O, ""},
#elif PY_VERSION_HEX >= 0x02020000
        {"xparse", (PyCFunction) Py3hp_Core_Parser_Func, METH_O, ""},
#else
#endif
#if PY_VERSION_HEX >= 0x03000000
#elif PY_VERSION_HEX >= 0x02000000
        {"parse", (PyCFunction) Py3hp_Core_OldParser_Func, METH_O, ""},
#else
#endif
        {NULL}
};

#if PY_VERSION_HEX >= 0x03000000

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
#endif

#if PY_VERSION_HEX >= 0x03000000

PyMODINIT_FUNC PyInit_core(void)
#else
PyMODINIT_FUNC initcore(void)
#endif
{
    PyObject *module;
#if PY_VERSION_HEX >= 0x02020000
    if (PyType_Ready(&Py3hp_Core_ParserIterator_Type) || PyType_Ready(&Py3hp_Core_ParserMatch_Type))
    {
# if PY_VERSION_HEX >= 0x03000000
        return NULL;
# else
        return;
# endif
    }
#else
#endif
#if PY_VERSION_HEX >= 0x03000000
    module = PyModule_Create(&module_def);
#else
    module = Py_InitModule("py3hp.core", module_functions);
#endif
    if (module == NULL)
    {
#if PY_VERSION_HEX >= 0x03000000
        return NULL;
#else
        return;
#endif
    }
#if PY_VERSION_HEX >= 0x03000000
    return module;
#else
#endif
}
