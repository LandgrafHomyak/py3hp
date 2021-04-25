#include <Python.h>

#ifndef PY3HP_CODE_CLASS_H
#define PY3HP_CODE_CLASS_H

int Code_Class_Init(void);

void Code_Class_Free(void);

PyTypeObject Py3hpCode_Type;

typedef struct {
    PyObject_VAR_HEAD
    PyObject *file_name;
    PyObject *commands[1];
} Py3hpCode_Object;

PyObject *Py3hpCode_CompileS(PyObject *_cls_or_module, PyObject *args, PyObject *kwargs);

#endif /* PY3HP_CODE_CLASS_H */
