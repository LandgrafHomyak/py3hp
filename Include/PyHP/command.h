#include <Python.h>

#include "command_type.h"

#ifndef PyHP_COMMAND_H
#define PyHP_COMMAND_H

typedef struct PyHP_Command
{
    PyHP_CommandType type;
    PyObject *value;
} PyHP_Command;

typedef struct PyHP_Command_Object
{
    PyObject_HEAD
    PyHP_Command data;
} PyHP_Command_Object;

extern PyTypeObject PyHP_Command_Type;

int PyHP_Command_Converter(PyHP_Command_Object *src, PyHP_Command *dst);

#endif /* PyHP_COMMAND_H */
