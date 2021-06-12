#include <Python.h>
#include "class.h"

#ifndef PY3HP_CODE_ITERATOR_H
#define PY3HP_CODE_ITERATOR_H

int Code_Iterator_Init(PyObject *module);

void Code_Iterator_Free(void);

PyObject *Py3hpCode_Iter(Py3hpCode_Object *master);

PyObject *Py3hpCode_GetItem(Py3hpCode_Object *self, Py_ssize_t i);

#endif /* PY3HP_CODE_ITERATOR_H */
