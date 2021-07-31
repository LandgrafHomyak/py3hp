#include <Python.h>

#include <PyHP/modules.h>

PyMODINIT_FUNC PyInit_types(void)
{
    return PyHPInit_types();
}