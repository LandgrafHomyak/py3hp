#include <Python.h>

#include <PyHP/modules.h>

PyMODINIT_FUNC PyInit_executor(void)
{
    return PyHPInit_executor();
}