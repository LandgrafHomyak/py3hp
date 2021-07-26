#include <Python.h>

#include <PyHP.h>

static PyMethodDef module_methods[] = {
        {"align_code", (PyCFunction) PyHP_AlignCode_Func, METH_VARARGS | METH_KEYWORDS},
        {"parse",      (PyCFunction) PyHP_Parser_Func,    METH_O},
        {NULL}
};

static PyModuleDef module_def = {
        .m_name = "pyhp.parser",
        .m_methods = module_methods
};

PyMODINIT_FUNC PyInit_parser(void)
{
    PyObject *module;

    if (PyType_Ready(&PyHP_ParserIterator_Type))
    {
        return NULL;
    }
    if (PyType_Ready(&PyHP_ParserMatch_Type))
    {
        return NULL;
    }

    module = PyModule_Create(&module_def);
    if (module == NULL)
    {
        return NULL;
    }

    PyModule_AddObject(module, "parser_iterator", (PyObject *) &PyHP_ParserIterator_Type);
    PyModule_AddObject(module, "parser_match", (PyObject *) &PyHP_ParserMatch_Type);
    return module;
}