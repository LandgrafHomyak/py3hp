#include <Python.h>

#include "pool.h"

#include "pool/class.h"

int Pool_Init(PyObject *module)
{
    if (Pool_Class_Init(module))
    {
        return -1;
    }
    return 0;
}

void Pool_Free(void)
{
    Pool_Class_Free();
}