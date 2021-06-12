#include <Python.h>

#ifndef PY3HP_POOL_SUBINTERPETER_H
#define PY3HP_POOL_SUBINTERPETER_H
typedef struct
{
    PyObject_HEAD
    PyThreadState *ts;
} Py3hpSubInterpreter_Object;
#endif /* PY3HP_POOL_SUBINTERPETER_H */
