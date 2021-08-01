#include <Python.h>
#include <PyHP.h>
#include <PyHP/modules.h>

int PyHP_Init(void)
{
    PyHP_IteratorMeta_Type.tp_base = &PyType_Type;
    if (PyType_Ready(&PyHP_IteratorMeta_Type))
    {
        return -1;
    }
    if (PyType_Ready(&PyHP_StatementType_Type))
    {
        return -1;
    }
    if (PyType_Ready(&PyHP_ParserMatch_Type))
    {
        return -1;
    }
    /*if (PyType_Ready(&PyHP_ParserIterator_Type))
    {
        return -1;
    }
    if (PyType_Ready(&PyHP_ParserMatch_Type))
    {
        return -1;
    }
    PyHP_PreCompilerIterator_Type.tp_base = &PyHP_ParserIterator_Type;
    if (PyType_Ready(&PyHP_PreCompilerIterator_Type))
    {
        return -1;
    }
    if (PyType_Ready(&PyHP_PreCompilerCommand_Type))
    {
        return -1;
    }
    */
    return 0;
}


static PyMethodDef PyHPInit_parser_methods[] = {
        /*
        {"align_code", (PyCFunction) PyHP_AlignCode_Func, METH_VARARGS | METH_KEYWORDS},
        {"parse",      (PyCFunction) PyHP_Parser_Func,    METH_O},
        */
        {NULL}
};
static PyModuleDef PyHPInit_parser_def = {
        .m_name = "pyhp.parser",
        .m_methods = PyHPInit_parser_methods
};

PyMODINIT_FUNC PyHPInit_parser(void)
{
    PyObject *module;

    if (PyHP_Init() != 0)
    {
        return NULL;
    }

    module = PyModule_Create(&PyHPInit_parser_def);
    if (module == NULL)
    {
        return NULL;
    }
    /*
    PyModule_AddObject(module, "parser_iterator", (PyObject *) &PyHP_ParserIterator_Type);
    PyModule_AddObject(module, "parser_match", (PyObject *) &PyHP_ParserMatch_Type);
    */
    PyModule_AddObject(module, "NONE", (PyObject *) PyHP_StatementTypeObject_NONE);
    PyModule_AddObject(module, "RAW", (PyObject *) PyHP_StatementTypeObject_RAW);
    PyModule_AddObject(module, "INLINE", (PyObject *) PyHP_StatementTypeObject_INLINE);
    PyModule_AddObject(module, "BLOCK", (PyObject *) PyHP_StatementTypeObject_BLOCK);


    return module;
}
static PyModuleDef PyHPInit_types_def = {
        .m_name = "pyhp.types",
};

PyMODINIT_FUNC PyHPInit_types(void)
{
    PyObject *module;

    if (PyHP_Init() != 0)
    {
        return NULL;
    }

    module = PyModule_Create(&PyHPInit_types_def);
    if (module == NULL)
    {
        return NULL;
    }
    PyModule_AddObject(module, "iterator_meta", (PyObject *) &PyHP_IteratorMeta_Type);

    return module;
}
/*

static PyMethodDef PyHPInit_compiler_methods[] = {
        {NULL}
};

static PyModuleDef PyHPInit_compiler_def = {
        .m_name = "pyhp.compiler",
        .m_methods = PyHPInit_compiler_methods
};

PyMODINIT_FUNC PyHPInit_compiler(void)
{
    PyObject *module;

    if (PyHP_Init() != 0)
    {
        return NULL;
    }

    module = PyModule_Create(&PyHPInit_compiler_def);
    if (module == NULL)
    {
        return NULL;
    }

    PyModule_AddObject(module, "NONE", (PyObject *) &PyHP_CommandType_NONE);
    PyModule_AddObject(module, "TEXT", (PyObject *) &PyHP_CommandType_TEXT);
    PyModule_AddObject(module, "EVAl", (PyObject *) &PyHP_CommandType_EVAL);
    PyModule_AddObject(module, "EXEC", (PyObject *) &PyHP_CommandType_EXEC);

    PyModule_AddObject(module, "precompiler_iterator", (PyObject *) &PyHP_PreCompilerItertor_Type);
    PyModule_AddObject(module, "precompiler_command", (PyObject *) &PyHP_PreCompilerCommand_Type);

    return module;
}
*/
