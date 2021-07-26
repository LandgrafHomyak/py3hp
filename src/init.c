#include <Python.h>
#include <PyHP.h>
#include <PyHP/modules.h>

int PyHP_Init(void)
{
    if (PyType_Ready(&PyHP_ParserIterator_Type))
    {
        return -1;
    }
    if (PyType_Ready(&PyHP_ParserMatch_Type))
    {
        return -1;
    }
    return 0;
}


static PyMethodDef PyHPInit_parser_methods[] = {
        {"align_code", (PyCFunction) PyHP_AlignCode_Func, METH_VARARGS | METH_KEYWORDS},
        {"parse",      (PyCFunction) PyHP_Parser_Func,    METH_O},
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

    PyModule_AddObject(module, "parser_iterator", (PyObject *) &PyHP_ParserIterator_Type);
    PyModule_AddObject(module, "parser_match", (PyObject *) &PyHP_ParserMatch_Type);
    return module;
}