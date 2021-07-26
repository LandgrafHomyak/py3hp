#include <Python.h>

#include <PyHP/modules.h>

PyMODINIT_FUNC PyInit_parser(void)
{
    return PyHPInit_parser();
}