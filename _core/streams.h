#include <Python.h>

#ifndef PY3HP_CORE_STREAMS_H
#define PY3HP_CORE_STREAMS_H

typedef struct
{
    PyObject_HEAD
    char *buffer;
    Py_ssize_t len;
    Py_ssize_t allocated;
    Py_ssize_t pos;
} Py3hpCore_OutStream_Object;

extern PyTypeObject Py3hpCore_PageStdout_Type;
extern PyTypeObject Py3hpCore_PageStderr_Type;

Py_ssize_t Py3hpCore_OutStream_Write(Py3hpCore_OutStream_Object *self, char *string, Py_ssize_t len);
Py3hpCore_OutStream_Object *Py3hpCore_OutStream_Create(PyTypeObject *cls);

#endif /* PY3HP_CORE_STREAMS_H */
