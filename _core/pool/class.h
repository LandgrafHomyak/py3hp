#include <Python.h>
#include "subinterpeter.h"

#ifndef PY3HP_POOL_CLASS_H
#define PY3HP_POOL_CLASS_H

typedef struct
{
    PyObject_HEAD
    Py3hpSubInterpreter_Object *master;
    Py_ssize_t len;
    Py3hpSubInterpreter_Object **array;
    struct
    {
        int si_pre_init: 1;
    } flags;
} Py3hpPool_Object;

Py3hpPool_Object *Py3hpPool_FromMaster(PyThreadState *master);

void Py3hpPool_MakeDefault(Py3hpPool_Object *o);

int Pool_Class_Init(PyObject *module);

void Pool_Class_Free(void);

#endif /* PY3HP_POOL_CLASS_H */
