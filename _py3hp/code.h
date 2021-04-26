#include <Python.h>

#include "code/class.h"
#include "code/highlevel.h"

#ifndef PY3HP_CODE_H
#define PY3HP_CODE_H

int Code_Init(PyObject *module);

void Code_Free(void);

#endif /* _PY3HP_CODE_H */
