#include <Python.h>

#include "pool/highlevel.h"

#ifndef PY3HP_POOL_H
#define PY3HP_POOL_H

int Code_Init(PyObject *module);

void Code_Free(void);

#endif /* PY3HP_POOL_H */
