#include <Python.h>

#include "py3hp.h"
#include "encoding.h"

PY3HP_LOW_API const char *Py3hp_Core_EncodeStringRO(PyObject *object, Py_ssize_t *p_len)
{
    const char *raw;
    Py_ssize_t len;
#if PY_VERSION_HEX < 0x03030000
    PyObject *bytes;
#endif

    if (object == NULL)
    {
        PyErr_BadInternalCall();
        return NULL;
    }


#if PY_VERSION_HEX >= 0x03030000
    raw = PyUnicode_AsUTF8AndSize(object, &len);
#else
    bytes = PyUnicode_AsUTF8String(object);
    if (bytes == NULL)
    {
        return NULL;
    }
    len = PyBytes_GET_SIZE(bytes);
    raw = PyBytes_AS_STRING(bytes);
#endif
    if (raw == NULL)
    {
        return NULL;
    }

    if (p_len != NULL)
    {
        *p_len = len;
    }

    return raw;
}

PY3HP_LOW_API char *Py3hp_Core_EncodeString(PyObject *object, Py_ssize_t *p_len)
{
    const char *raw;
    Py_ssize_t len;
    char *buffer;

    raw = Py3hp_Core_EncodeStringRO(object, &len);

    buffer = PyMem_Malloc(len + 1);
    if (buffer == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }
    memcpy(buffer, raw, len + 1);

    if (p_len != NULL)
    {
        *p_len = len;
    }

    return buffer;
}


PY3HP_LOW_API PyObject *Py3hp_Core_DecodeString(const char *raw, Py_ssize_t start, Py_ssize_t len)
{
    PyObject *object;

    if (raw == NULL || len <= 0 || start < 0)
    {
        PyErr_BadInternalCall();
        return NULL;
    }

    object = PyUnicode_DecodeUTF8(raw + start, len, NULL);
    if (object == NULL)
    {
        return NULL;
    }

    return object;
};