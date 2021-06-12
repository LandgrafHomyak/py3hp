#include <Python.h>

#ifndef PY3HP_POOL_THREAD_H
#define PY3HP_POOL_THREAD_H
typedef enum
{
    Py3hpSubInterpreterThread_Status_CREATED = 0,
    Py3hpSubInterpreterThread_Status_INITIALIZING = 1,
    Py3hpSubInterpreterThread_Status_INITIALIZED = 2,
    Py3hpSubInterpreterThread_Status_RUNNING = 3,
    Py3hpSubInterpreterThread_Status_FINISHED = 4,
    Py3hpSubInterpreterThread_Status_FINALIZING = 5,
    Py3hpSubInterpreterThread_Status_FINALIZED = 6,
} Py3hpSubInterpreterThread_Status;


typedef struct
{
    PyObject_HEAD;
    PyThreadState *master;
    PyThreadState *interpreter;
    Py3hpSubInterpreterThread_Status status;
} Py3hpSubInterpreterThread_Object;

#endif /* PY3HP_POOL_THREAD_H */
