#include <Python.h>

#ifndef PyHP_API_ENCODING_H
# define PyHP_API_ENCODING_H

const char *PyHP_API_EncodeStringRO(PyObject *object, Py_ssize_t *p_len);

char *PyHP_API_EncodeString(PyObject *object, Py_ssize_t *p_len);

PyObject *PyHP_API_DecodeString(const char *raw, Py_ssize_t start, Py_ssize_t len);

#endif /* PyHP_API_ENCODING_H */
