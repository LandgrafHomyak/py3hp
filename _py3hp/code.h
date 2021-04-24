#include <Python.h>

#ifndef PY3HP_CODE_H
#define PY3HP_CODE_H

extern PyTypeObject *StringIO_Type;

PyObject *Code_Compile(PyTypeObject *cls, PyObject *source);

#endif PY3HP_CODE_H
