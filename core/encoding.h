#include <Python.h>
#include "pyhp.h"

#ifndef PyHP_CORE_ENCODING_H
# define PyHP_CORE_ENCODING_H

PyHP_LOW_API const char *PyHP_Core_EncodeStringRO(PyObject *object, Py_ssize_t *p_len);

PyHP_LOW_API char *PyHP_Core_EncodeString(PyObject *object, Py_ssize_t *p_len);

PyHP_LOW_API PyObject *PyHP_Core_DecodeString(const char *raw, Py_ssize_t start, Py_ssize_t len);

#endif /* PyHP_CORE_ENCODING_H */
