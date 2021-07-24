#include <Python.h>
#include <PyHP/core.h>

PyMODINIT_FUNC PyInit_core(void)
{
    return PyHP_Core_CreateModule();
}