#include <Python.h>

#ifndef PY3HP_CODE_COMMANDTYPE_H
#define PY3HP_CODE_COMMANDTYPE_H

int Code_CommandType_Init(PyObject *module);

void Code_CommandType_Free(void);

typedef enum
{
    Py3hpCode_Command_NOT_FOUND = -1,

    Py3hpCode_Command_RAW = 0,
    Py3hpCode_Command_EVAL = 1,
    Py3hpCode_Command_EXEC = 2
} Py3hpCode_Command_Type;


PyObject *Py3hpCode_CommandType_Get(Py3hpCode_Command_Type id);

#endif /* PY3HP_CODE_COMMANDTYPE_H */
