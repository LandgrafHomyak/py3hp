#include <Python.h>

#include <PyHP/modules.h>

PyMODINIT_FUNC PyInit_compiler(void)
{
    return PyHPInit_compiler();
}