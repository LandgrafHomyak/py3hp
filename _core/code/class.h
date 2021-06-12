#include <Python.h>
#include "commandtype.h"

#ifndef PY3HP_CODE_CLASS_H
#define PY3HP_CODE_CLASS_H

int Code_Class_Init(PyObject *module);

void Code_Class_Free(void);

typedef struct
{
    Py3hpCode_Command_Type type;
    Py_ssize_t len;
    union
    {
        void *any;
        char *raw;
        PyObject *code;
    } pointer;
} Py3hpCode_Command;

typedef struct
{
    PyObject_VAR_HEAD
    PyObject *file_name;
    char *buffer;
    Py3hpCode_Command commands[1];
} Py3hpCode_Object;

PyTypeObject Py3hpCodeBytes_Type;
PyTypeObject Py3hpCodeStr_Type;


PyObject *Py3hpCode_CompileS(PyObject *_cls_or_module, PyObject *args, PyObject *kwargs);

#endif /* PY3HP_CODE_CLASS_H */
