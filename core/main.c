#include <Python.h>

#include "py3hp.h"
#include "parser.h"
#include "compiler.h"

static PyMethodDef module_functions[] = {
        {"parse",   (PyCFunction) Py3hp_Core_Parser_Func,    METH_O, ""},
        {"align",   (PyCFunction) Py3hp_Core_AlignCode_Func, METH_O, ""},
        {"compile", (PyCFunction) Py3hp_Core_Compile_Func,   METH_VARARGS | METH_KEYWORDS, ""},
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

    if (
            PyType_Ready(&Py3hp_Core_ParserIterator_Type) ||
            PyType_Ready(&Py3hp_Core_ParserMatch_Type) ||
            PyType_Ready(&Py3hp_Core_PageCode_Type) ||
            PyType_Ready(&Py3hp_Core_PageCodeIterator_Type)
            )
    {
        return NULL;
    }
    module = PyModule_Create(&module_def);
    if (module == NULL)
    {
        return NULL;
    }

#define ADD_PARSER_ENUM_VALUE(NAME) if(PyModule_AddIntConstant(module,#NAME,Py3hp_Core_StatementType_##NAME)!=0){return NULL;}
    ADD_PARSER_ENUM_VALUE(RAW)
    ADD_PARSER_ENUM_VALUE(INLINE)
    ADD_PARSER_ENUM_VALUE(BLOCK)
    ADD_PARSER_ENUM_VALUE(INLINE3)
    ADD_PARSER_ENUM_VALUE(BLOCK3)
    ADD_PARSER_ENUM_VALUE(INLINE2)
    ADD_PARSER_ENUM_VALUE(BLOCK2)
    ADD_PARSER_ENUM_VALUE(INLINE1)
    ADD_PARSER_ENUM_VALUE(BLOCK1)
#undef ADD_PARSER_ENUM_VALUE

#define ADD_COMPILER_ENUM_VALUE(NAME) if(PyModule_AddIntConstant(module,#NAME,Py3hp_Core_PageCodeType_##NAME)!=0){return NULL;}
    ADD_COMPILER_ENUM_VALUE(TEXT)
    ADD_COMPILER_ENUM_VALUE(EXEC)
    ADD_COMPILER_ENUM_VALUE(EVAL)
#undef ADD_COMPILER_ENUM_VALUE

    return module;
}
