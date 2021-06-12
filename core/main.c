#include <Python.h>

#include "py3hp.h"
#include "parser.h"

static PyMethodDef module_functions[] = {
        {"parse", (PyCFunction) Py3hp_Core_Parser_Func, METH_O, ""},
        {"align", (PyCFunction) Py3hp_Core_AlignCode_Func, METH_O, ""},
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
    PyObject *temp_object;
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
#define ADD_PARSER_ENUM_VALUE(NAME) if(PyModule_AddIntConstant(module,#NAME,Py3hp_Core_StatementType_##NAME)!=0){return NULL;}
#else
#define ADD_PARSER_ENUM_VALUE(NAME) if(PyModule_AddIntConstant(module,#NAME,Py3hp_Core_StatementType_##NAME)!=0){return;}
#endif
    ADD_PARSER_ENUM_VALUE(RAW);
    ADD_PARSER_ENUM_VALUE(INLINE);
    ADD_PARSER_ENUM_VALUE(BLOCK);
    ADD_PARSER_ENUM_VALUE(INLINE3);
    ADD_PARSER_ENUM_VALUE(BLOCK3);
    ADD_PARSER_ENUM_VALUE(INLINE2);
    ADD_PARSER_ENUM_VALUE(BLOCK2);
    ADD_PARSER_ENUM_VALUE(INLINE1);
    ADD_PARSER_ENUM_VALUE(BLOCK1);
#undef ADD_PARSER_ENUM_VALUE

#if PY_VERSION_HEX >= 0x03000000
    return module;
#else
#endif
}
