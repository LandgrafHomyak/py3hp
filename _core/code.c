#include <Python.h>

#include "code.h"

#include "code/class.h"
#include "code/iterator.h"
#include "code/commandtype.h"

int Code_Init(PyObject *module)
{
    if (Code_Class_Init(module))
    {
        return -1;
    }
    if (Code_CommandType_Init(module))
    {
        return -1;
    }
    if (Code_Iterator_Init(module))
    {
        return -1;
    }

    return 0;
}

void Code_Free(void)
{
    Code_Class_Free();
    Code_CommandType_Free();
    Code_Iterator_Free();
}