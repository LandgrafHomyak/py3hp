#include <Python.h>
#include <PyHP.h>

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