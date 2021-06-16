#include <Python.h>

#include "pyhp.h"
#include "parser.h"
#include "compiler.h"
#include "streams.h"
#include "executor.h"

static PyMethodDef module_functions[] = {
        {"parse",   (PyCFunction) PyHP_Core_Parser_Func,    METH_O,               ""},
        {"align",   (PyCFunction) PyHP_Core_AlignCode_Func, METH_O,               ""},
        {"compile", (PyCFunction) PyHP_Core_Compile_Func,   METH_VARARGS | METH_KEYWORDS, ""},
        {"_exec",   (PyCFunction) PyHP_Core_ExecEmbed_Func, METH_O,               ""},
        {"exec",    (PyCFunction) PyHP_Core_Exec_Func, METH_VARARGS | METH_KEYWORDS, ""},
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
            PyType_Ready(&PyHP_Core_ParserIterator_Type) ||
            PyType_Ready(&PyHP_Core_ParserMatch_Type) ||
            PyType_Ready(&PyHP_Core_PageCode_Type) ||
            PyType_Ready(&PyHP_Core_PageCodeIterator_Type) ||
            PyType_Ready(&PyHP_Core_BaseStream_Type) ||
            PyType_Ready(&PyHP_Core_StdinPreset_Type) ||
            PyType_Ready(&PyHP_Core_Stdout_Type) ||
            PyType_Ready(&PyHP_Core_Stderr_Type) ||
            PyType_Ready(&PyHP_Core_StdoutPipe_Type) ||
            PyType_Ready(&PyHP_Core_StderrPipe_Type)
            )
    {
        return NULL;
    }
    module = PyModule_Create(&module_def);
    if (module == NULL)
    {
        return NULL;
    }

#define ADD_PARSER_ENUM_VALUE(NAME) if(PyModule_AddIntConstant(module,#NAME,PyHP_Core_StatementType_##NAME)!=0){return NULL;}
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

#define ADD_COMPILER_ENUM_VALUE(NAME) if(PyModule_AddIntConstant(module,#NAME,PyHP_Core_PageCodeType_##NAME)!=0){return NULL;}
    ADD_COMPILER_ENUM_VALUE(TEXT)
    ADD_COMPILER_ENUM_VALUE(EXEC)
    ADD_COMPILER_ENUM_VALUE(EVAL)
#undef ADD_COMPILER_ENUM_VALUE

    if (PyModule_AddObject(module, "_base_stream", (PyObject *) (&PyHP_Core_BaseStream_Type)) != 0)
    {
        return NULL;
    }

    return module;
}
