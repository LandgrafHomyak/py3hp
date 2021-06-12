#include <Python.h>

#include "encoding.h"

PyObject *Py3hpCore_DecodeToBytes(char *string, Py_ssize_t len)
{
    PyObject *bytes;
    bytes = PyBytes_FromStringAndSize(string, len);
    if (bytes == NULL)
    {
        return NULL;
    }

    return bytes;
}

PyObject *Py3hpCore_DecodeToString_O(PyObject *bytes)
{
    PyObject *string;
    string = PyUnicode_FromEncodedObject(bytes, "utf-8", NULL);
    if (string == NULL)
    {
        return NULL;
    }

    return string;
}

PyObject *Py3hpCore_DecodeToString(char *source, Py_ssize_t len)
{
    PyObject *bytes;
    PyObject *string;

    bytes = Py3hpCore_DecodeToBytes(source, len);
    if (bytes == NULL)
    {
        return NULL;
    }

    string = Py3hpCore_DecodeToString_O(bytes);
    Py_DECREF(string);
    if (string == NULL)
    {
        return NULL;
    }

    return string;
}

string_and_len Py3hpCore_EncodeFromBytes(PyObject *bytes)
{
    string_and_len encoded;

    encoded.len = PyBytes_GET_SIZE(bytes); /* zero char at the len(bytes) + 1 don't required because struct stores buffer's len */
    encoded.buffer = PyMem_Malloc(encoded.len);
    if (encoded.buffer == NULL)
    {

        PyErr_NoMemory();
        Py3hpCore__StringAndLen_SetErr(encoded);
        return encoded;
    }
    memcpy(encoded.buffer, PyBytes_AS_STRING(bytes), encoded.len);

    return encoded;
}

PyObject *Py3hpCore_EncodeFromString_O(PyObject *string)
{
    PyObject *bytes;
    bytes = PyUnicode_AsEncodedString(string, "utf-8", NULL);
    if (bytes == NULL)
    {
        return NULL;
    }

    return bytes;
}

string_and_len Py3hpCore_EncodeFromString(PyObject *string)
{
    string_and_len encoded;
    PyObject *bytes;

    bytes = Py3hpCore_EncodeFromString_O(string);
    if (bytes == NULL)
    {
        Py3hpCore__StringAndLen_SetErr(encoded);
        return encoded;
    }

    encoded = Py3hpCore_EncodeFromBytes(bytes);
    Py_DECREF(string);
    if (Py3hpCore__StringAndLen_CheckErr(encoded))
    {
        return encoded;
    }

    return encoded;
}