#include <Python.h>
#include "py3hp.h"

#ifndef PY3HP_CORE_ENCODING_H
# define PY3HP_CORE_ENCODING_H

PY3HP_LOW_API const char *Py3hp_Core_EncodeStringRO(PyObject *object, Py_ssize_t *p_len);

PY3HP_LOW_API char *Py3hp_Core_EncodeString(PyObject *object, Py_ssize_t *p_len);

PY3HP_LOW_API PyObject *Py3hp_Core_DecodeString(char *raw, Py_ssize_t start, Py_ssize_t len);

#endif /* PY3HP_CORE_ENCODING_H */
