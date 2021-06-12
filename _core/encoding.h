#ifndef PY3HP_CORE_ENCODING_H
#define PY3HP_CORE_ENCODING_H


typedef struct {
    Py_ssize_t len;
    char *buffer;
} string_and_len;

#define Py3hpCore__StringAndLen_SetErr(VAR) ((VAR).len = -1)
#define Py3hpCore__StringAndLen_CheckErr(VAR) ((VAR).len == -1)

PyObject *Py3hpCore_DecodeToBytes(char *string, Py_ssize_t len);

PyObject *Py3hpCore_DecodeToString_O(PyObject *bytes);

PyObject *Py3hpCore_DecodeToString(char *source, Py_ssize_t len);

string_and_len Py3hpCore_EncodeFromBytes(PyObject *bytes);

PyObject *Py3hpCore_EncodeFromString_O(PyObject *string);

string_and_len Py3hpCore_EncodeFromString(PyObject *string);

#endif /* PY3HP_CORE_ENCODING_H */
