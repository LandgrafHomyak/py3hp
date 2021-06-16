#include <Python.h>
#include "pyhp.h"
#include "streams.h"
#include "encoding.h"
#include <stdio.h>

PyHP_LOW_API int PyHP_Core_Stream_Extend(PyHP_Core_Stream *stream, Py_ssize_t new_size)
{
    if (new_size < stream->allocated)
    {
        return 0;
    }

    if (stream->allocated * 2 < new_size)
    {
        stream->allocated = new_size;
    }
    else
    {
        stream->allocated *= 2;
    }
    stream->buffer = PyMem_Realloc(stream->buffer, stream->allocated);
    if (stream->buffer == NULL)
    {
        PyErr_NoMemory();
        return 1;
    }

    return 0;
}

PyHP_LOW_API int PyHP_Core_Stream_Write(PyHP_Core_Stream *stream, const char *string, Py_ssize_t start, Py_ssize_t len)
{
    if (PyHP_Core_Stream_Extend(stream, stream->pos + len) != 0)
    {
        return 1;
    }

    if (len < 0)
    {
        PyErr_BadInternalCall();
        return 1;
    }
    memcpy(stream->buffer + stream->endpos, string + start, len);

    return 0;
}

PyHP_LOW_API int PyHP_Core_Stream_Seek(PyHP_Core_Stream *stream, Py_ssize_t pos, int whence)
{
    switch (whence)
    {
        case SEEK_SET:
            break;
        case SEEK_CUR:
            pos += stream->pos;
            break;
        case SEEK_END:
            pos += stream->endpos;
            break;
        default:
            PyErr_BadInternalCall();
            return 1;
    }

    if (pos < 0)
    {
        PyErr_BadInternalCall();
        return 1;
    }

    if (PyHP_Core_Stream_Extend(stream, pos) != 0)
    {
        return 1;
    }
    if (pos > stream->endpos)
    {
        memset(stream->buffer + stream->endpos, 0, pos - stream->endpos);
        stream->endpos = pos;
    }
    stream->pos = pos;

    return 0;
}

PyHP_LOW_API const char *PyHP_Core_Stream_Read(PyHP_Core_Stream *stream, Py_ssize_t *len)
{
    register char *ptr;
    if (len != NULL)
    {
        *len = stream->endpos - stream->pos;
    }
    ptr = stream->buffer + stream->pos;
    stream->pos = stream->endpos;
    return ptr;
}


PyHP_LOW_API const char *PyHP_Core_Stream_ReadN(PyHP_Core_Stream *stream, Py_ssize_t *len)
{
    register char *ptr;
    if (len == NULL || *len < 0)
    {
        PyErr_BadInternalCall();
        return NULL;
    }
    if (stream->endpos < stream->pos + *len)
    {
        *len = stream->endpos - stream->pos;
    }
    ptr = stream->buffer + stream->pos;
    stream->pos += *len;
    return ptr;
}

PyHP_LOW_API int PyHP_Core_Stream_Open(PyHP_Core_Stream *stream)
{
    stream->buffer = NULL;
    stream->pos = 0;
    stream->endpos = 0;
    stream->allocated = 0;
    return 0;
}

PyHP_LOW_API int PyHP_Core_Stream_Close(PyHP_Core_Stream *stream)
{
    if (stream->buffer != NULL)
    {
        PyMem_Free(stream->buffer);
    }
    return 0;
}

PyTypeObject PyHP_Core_BaseStream_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core._base_stream"
};

static void PyHP_Core_StdinPreset_Dealloc(PyHP_Core_StdinPreset_Object *self)
{
    Py_XDECREF(self->string);
    Py_TYPE(self)->tp_free(self);
}

#define PyHP_Core_BaseStream_ClosedErr() (PyErr_Format(PyExc_ValueError, "I/O operation on closed file."))

static PyObject *PyHP_Core_StdinPreset_Read(PyHP_Core_StdinPreset_Object *self, PyObject *args)
{
    Py_ssize_t end;
    PyObject *slice;

    if (self->string == NULL)
    {
        PyHP_Core_BaseStream_ClosedErr();
        return NULL;
    }

    if (PyTuple_Size(args) > 0)
    {
        if (!PyArg_ParseTuple(args, "|n", &end))
        {
            return NULL;
        }
        if (end < 0)
        {
            PyErr_Format(
                    PyExc_ValueError,
                    "can't read negative count of chars"
            );
            return NULL;
        }
#if PY_VERSION_HEX > 0x03030000
        if (PyUnicode_GET_LENGTH(self->string) - self->pos > end)
        {
            end = PyUnicode_GET_LENGTH(self->string);
        }
#else
        if (PyUnicode_GET_SIZE(self->string) - self->pos > end)
        {
            end = PyUnicode_GET_SIZE(self->string);
        }
#endif
        else
        {
            end += self->pos;
        }
    }
    else
    {
#if PY_VERSION_HEX > 0x03030000
        end = PyUnicode_GET_LENGTH(self->string);
#else
        end = PyUnicode_GET_SIZE(self->string);
#endif
    }

#if PY_VERSION_HEX > 0x03030000
    slice = PyUnicode_Substring(self->string, self->pos, end);
#else
    slice = PySequence_GetSlice(self->string, self->pos, end);
#endif
    if (slice == NULL)
    {
        return NULL;
    }

    self->pos = end;
    return slice;
}

static PyObject *PyHP_Core_StdinPreset_Readline(PyHP_Core_StdinPreset_Object *self, PyObject *args)
{
    Py_ssize_t end;
    PyObject *slice;
#if PY_VERSION_HEX < 0x03030000
    static Py_UNICODE substr_data[1] = {'\n'};
    static PyUnicodeObject substr = {
            PyObject_HEAD_INIT(&PyUnicode_Type)
            1, /* length */
            (Py_UNICODE *) (&substr_data), /* str */
            -1, /* hash */
            0, /* state */
            NULL, /* defenc */
    };
#endif

    if (self->string == NULL)
    {
        PyHP_Core_BaseStream_ClosedErr();
        return NULL;
    }

    if (PyTuple_Size(args) > 0)
    {
        if (!PyArg_ParseTuple(args, "|n", &end))
        {
            return NULL;
        }
        if (end < 0)
        {
            PyErr_Format(
                    PyExc_ValueError,
                    "can't read negative count of chars"
            );
            return NULL;
        }
#if PY_VERSION_HEX > 0x03030000
        if (PyUnicode_GET_LENGTH(self->string) - self->pos > end)
        {
            end = PyUnicode_GET_LENGTH(self->string);
        }
#else
        if (PyUnicode_GET_SIZE(self->string) - self->pos > end)
        {
            end = PyUnicode_GET_SIZE(self->string);
        }
#endif
        else
        {
            end += self->pos;
        }
    }
    else
    {
#if PY_VERSION_HEX > 0x03030000
        end = PyUnicode_GET_LENGTH(self->string);
#else
        end = PyUnicode_GET_SIZE(self->string);
#endif
    }

#if PY_VERSION_HEX >= 0x03030000
    end = PyUnicode_FindChar(self->string, '\n', self->pos, end, 1);
#else
    end = PyUnicode_Find(self->string, (PyObject *) &substr, self->pos, end, 1);
#endif

    switch (end)
    {
        case -2:
            return NULL;
        case -1:
#if PY_VERSION_HEX > 0x03030000
            end = PyUnicode_GET_LENGTH(self->string);
#else
            end = PyUnicode_GET_SIZE(self->string);
#endif
            break;
        default:
            end++;
            break;
    }

#if PY_VERSION_HEX > 0x03030000
    slice = PyUnicode_Substring(self->string, self->pos, end);
#else
    slice = PySequence_GetSlice(self->string, self->pos, end);
#endif
    if (slice == NULL)
    {
        return NULL;
    }

    self->pos = end;

    return slice;
}

static PyObject *PyHP_Core_StdinPreset_Close(PyHP_Core_StdinPreset_Object *self)
{
    self->string = NULL;
    Py_RETURN_NONE;
}

static PyObject *PyHP_Core_StdinPreset_FileNo(PyHP_Core_StdinPreset_Object *self)
{
    return PyLong_FromLong(0);
}

static PyObject *PyHP_Core_StdinPreset_Flush(PyHP_Core_StdinPreset_Object *self)
{
    Py_RETURN_NONE;
}

static PyObject *PyHP_Core_StdinPreset_Isatty(PyHP_Core_StdinPreset_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_StdinPreset_Readable(PyHP_Core_StdinPreset_Object *self)
{
    Py_RETURN_TRUE;
}

static PyObject *PyHP_Core_StdinPreset_Writeable(PyHP_Core_StdinPreset_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_StdinPreset_Seekable(PyHP_Core_StdinPreset_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_StdinPreset_Tell(PyHP_Core_StdinPreset_Object *self)
{
    return PyLong_FromSsize_t(self->pos);
}

static PyObject *PyHP_Core_StdinPreset_GetValue(PyHP_Core_StdinPreset_Object *self)
{
    Py_INCREF(self->string);
    return self->string;
}

static PyMethodDef PyHP_Core_StdinPreset_Methods[] = {
        {"readline",   (PyCFunction) PyHP_Core_StdinPreset_Readline,  METH_VARARGS, ""},
        {"read",       (PyCFunction) PyHP_Core_StdinPreset_Read,      METH_VARARGS, ""},
        {"close",      (PyCFunction) PyHP_Core_StdinPreset_Close,     METH_NOARGS,  ""},
        {"fileno",     (PyCFunction) PyHP_Core_StdinPreset_FileNo,    METH_NOARGS,  ""},
        {"flush",      (PyCFunction) PyHP_Core_StdinPreset_Flush,     METH_NOARGS,  ""},
        {"isatty",     (PyCFunction) PyHP_Core_StdinPreset_Isatty,    METH_NOARGS,  ""},
        {"readable",   (PyCFunction) PyHP_Core_StdinPreset_Readable,  METH_NOARGS,  ""},
        {"writeable",  (PyCFunction) PyHP_Core_StdinPreset_Writeable, METH_NOARGS,  ""},
        {"readlines",  (PyCFunction) NULL,                             METH_VARARGS, ""},
        {"seekable",   (PyCFunction) PyHP_Core_StdinPreset_Seekable,  METH_NOARGS,  ""},
        {"seek",       (PyCFunction) NULL,                             METH_VARARGS | METH_KEYWORDS, ""},
        {"tell",       (PyCFunction) PyHP_Core_StdinPreset_Tell,      METH_NOARGS,  ""},
        {"truncate",   (PyCFunction) NULL,                             METH_VARARGS, ""},
        {"write",      (PyCFunction) NULL,                             METH_O,       ""},
        {"writelines", (PyCFunction) NULL,                             METH_O,       ""},
        {"detach",     (PyCFunction) NULL,                             METH_NOARGS,  ""},
        {"getvalue",   (PyCFunction) PyHP_Core_StdinPreset_GetValue,  METH_NOARGS,  ""},
        {NULL}
};

static PyObject *PyHP_Core_StdinPreset_GetClosed(PyHP_Core_StdinPreset_Object *self)
{
    if (self->string == NULL)
    {
        Py_RETURN_TRUE;
    }
    else
    {
        Py_RETURN_FALSE;
    }
}

static PyObject *PyHP_Core_StdinPreset_GetMode(PyHP_Core_StdinPreset_Object *self)
{
    return PyUnicode_FromString("rt");
}

static PyObject *PyHP_Core_StdinPreset_GetName(PyHP_Core_StdinPreset_Object *self)
{
    return PyUnicode_FromString("<stdin>");
}

static PyObject *PyHP_Core_StdinPreset_GetEncoding(PyHP_Core_StdinPreset_Object *self)
{
    Py_RETURN_NONE;
}

static PyObject *PyHP_Core_StdinPreset_GetErrors(PyHP_Core_StdinPreset_Object *self)
{
    Py_RETURN_NONE;
}

static PyObject *PyHP_Core_StdinPreset_GetNewlines(PyHP_Core_StdinPreset_Object *self)
{
    return PyUnicode_FromString("\n");
}

static PyGetSetDef PyHP_Core_StdinPreset_GetSet[] = {
        {"closed",   (getter) PyHP_Core_StdinPreset_GetClosed,   NULL, ""},
        {"mode",     (getter) PyHP_Core_StdinPreset_GetMode,     NULL, ""},
        {"name",     (getter) PyHP_Core_StdinPreset_GetName,     NULL, ""},
        {"encoding", (getter) PyHP_Core_StdinPreset_GetEncoding, NULL, ""},
        {"errors",   (getter) PyHP_Core_StdinPreset_GetErrors,   NULL, ""},
        {"newlines", (getter) PyHP_Core_StdinPreset_GetNewlines, NULL, ""},
        {NULL}
};

PyTypeObject PyHP_Core_StdinPreset_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.stdin",
        .tp_base = &PyHP_Core_BaseStream_Type,
        .tp_basicsize = sizeof(PyHP_Core_StdinPreset_Object),
        .tp_dealloc = (destructor) PyHP_Core_StdinPreset_Dealloc,
        .tp_methods = PyHP_Core_StdinPreset_Methods,
        .tp_getset = PyHP_Core_StdinPreset_GetSet
};


static void PyHP_Core_Stdout_Dealloc(PyHP_Core_Stdout_Object *self)
{
    PyHP_Core_Stream_Close(&(self->data));
    Py_TYPE(self)->tp_free(self);
}

static PyObject *PyHP_Core_Stdout_Close(PyHP_Core_Stdout_Object *self)
{
    self->data.allocated = -1;
    Py_RETURN_NONE;
}

static PyObject *PyHP_Core_Stdout_FileNo(PyHP_Core_Stdout_Object *self)
{
    return PyLong_FromLong(1);
}

static PyObject *PyHP_Core_Stdout_Flush(PyHP_Core_Stdout_Object *self)
{
    Py_RETURN_NONE;
}

static PyObject *PyHP_Core_Stdout_Isatty(PyHP_Core_Stdout_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_Stdout_Readable(PyHP_Core_Stdout_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_Stdout_Writeable(PyHP_Core_Stdout_Object *self)
{
    Py_RETURN_TRUE;
}

static PyObject *PyHP_Core_Stdout_Seekable(PyHP_Core_Stdout_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_Stdout_Tell(PyHP_Core_Stdout_Object *self)
{
    return PyLong_FromSsize_t(self->data.endpos);
}

static PyObject *PyHP_Core_Stdout_Write(PyHP_Core_Stdout_Object *self, PyObject *string)
{
    const char *raw;
    Py_ssize_t len;

    if (self->data.allocated == -1)
    {
        PyHP_Core_BaseStream_ClosedErr();
        return NULL;
    }

    if (!PyUnicode_Check(string))
    {
        PyErr_Format(
                PyExc_TypeError,
                "can write only strings"
        );
        return NULL;
    }

    raw = PyHP_Core_EncodeStringRO(string, &len);
    if (raw == NULL)
    {
        return NULL;
    }

    if (PyHP_Core_Stream_Write(&self->data, raw, 0, len) != 0)
    {
        return NULL;
    }

    return PyLong_FromSsize_t(len);
}

static PyMethodDef PyHP_Core_Stdout_Methods[] = {
        {"readline",   (PyCFunction) NULL,                        METH_VARARGS, ""},
        {"read",       (PyCFunction) NULL,                        METH_VARARGS, ""},
        {"close",      (PyCFunction) PyHP_Core_Stdout_Close,     METH_NOARGS,  ""},
        {"fileno",     (PyCFunction) PyHP_Core_Stdout_FileNo,    METH_NOARGS,  ""},
        {"flush",      (PyCFunction) PyHP_Core_Stdout_Flush,     METH_NOARGS,  ""},
        {"isatty",     (PyCFunction) PyHP_Core_Stdout_Isatty,    METH_NOARGS,  ""},
        {"readable",   (PyCFunction) PyHP_Core_Stdout_Readable,  METH_NOARGS,  ""},
        {"writeable",  (PyCFunction) PyHP_Core_Stdout_Writeable, METH_NOARGS,  ""},
        {"readlines",  (PyCFunction) NULL,                        METH_VARARGS, ""},
        {"seekable",   (PyCFunction) PyHP_Core_Stdout_Seekable,  METH_NOARGS,  ""},
        {"seek",       (PyCFunction) NULL,                        METH_VARARGS | METH_KEYWORDS, ""},
        {"tell",       (PyCFunction) PyHP_Core_Stdout_Tell,      METH_NOARGS,  ""},
        {"truncate",   (PyCFunction) NULL,                        METH_VARARGS, ""},
        {"write",      (PyCFunction) PyHP_Core_Stdout_Write,     METH_O,       ""},
        {"writelines", (PyCFunction) NULL,                        METH_O,       ""},
        {"detach",     (PyCFunction) NULL,                        METH_NOARGS,  ""},
        {NULL}
};

static PyObject *PyHP_Core_Stdout_GetClosed(PyHP_Core_Stdout_Object *self)
{
    if (self->data.pos == -1)
    {
        Py_RETURN_TRUE;
    }
    else
    {
        Py_RETURN_FALSE;
    }
}

static PyObject *PyHP_Core_Stdout_GetMode(PyHP_Core_Stdout_Object *self)
{
    return PyUnicode_FromString("wt");
}

static PyObject *PyHP_Core_Stdout_GetName(PyHP_Core_Stdout_Object *self)
{
    return PyUnicode_FromString("<stdout>");
}

static PyObject *PyHP_Core_Stdout_GetEncoding(PyHP_Core_Stdout_Object *self)
{
    return PyUnicode_FromString("utf-8");
}

static PyObject *PyHP_Core_Stdout_GetErrors(PyHP_Core_Stdout_Object *self)
{
    return PyUnicode_FromString("strict");
}

static PyObject *PyHP_Core_Stdout_GetNewlines(PyHP_Core_Stdout_Object *self)
{
    return PyUnicode_FromString("\n");
}

static PyObject *PyHP_Core_Stdout_GetBuffer(PyHP_Core_Stdout_Object *self)
{
    return PyBytes_FromStringAndSize(self->data.buffer, self->data.endpos);
}

static PyGetSetDef PyHP_Core_Stdout_GetSet[] = {
        {"closed",   (getter) PyHP_Core_Stdout_GetClosed,   NULL, ""},
        {"mode",     (getter) PyHP_Core_Stdout_GetMode,     NULL, ""},
        {"name",     (getter) PyHP_Core_Stdout_GetName,     NULL, ""},
        {"encoding", (getter) PyHP_Core_Stdout_GetEncoding, NULL, ""},
        {"errors",   (getter) PyHP_Core_Stdout_GetErrors,   NULL, ""},
        {"newlines", (getter) PyHP_Core_Stdout_GetNewlines, NULL, ""},
        {"buffer",   (getter) PyHP_Core_Stdout_GetBuffer,   NULL, ""},
        {NULL}
};

PyTypeObject PyHP_Core_Stdout_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.stdout",
        .tp_base = &PyHP_Core_BaseStream_Type,
        .tp_basicsize = sizeof(PyHP_Core_Stdout_Object),
        .tp_dealloc = (destructor) PyHP_Core_Stdout_Dealloc,
        .tp_methods = PyHP_Core_Stdout_Methods,
        .tp_getset = PyHP_Core_Stdout_GetSet,
};


static void PyHP_Core_Stderr_Dealloc(PyHP_Core_Stdout_Object *self)
{
    PyHP_Core_Stream_Close(&(self->data));
    Py_TYPE(self)->tp_free(self);
}


static PyObject *PyHP_Core_Stderr_Close(PyHP_Core_Stderr_Object *self)
{
    self->data.allocated = -1;
    Py_RETURN_NONE;
}

static PyObject *PyHP_Core_Stderr_FileNo(PyHP_Core_Stderr_Object *self)
{
    return PyLong_FromLong(2);
}

static PyObject *PyHP_Core_Stderr_Flush(PyHP_Core_Stderr_Object *self)
{
    Py_RETURN_NONE;
}

static PyObject *PyHP_Core_Stderr_Isatty(PyHP_Core_Stderr_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_Stderr_Readable(PyHP_Core_Stderr_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_Stderr_Writeable(PyHP_Core_Stderr_Object *self)
{
    Py_RETURN_TRUE;
}

static PyObject *PyHP_Core_Stderr_Seekable(PyHP_Core_Stderr_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_Stderr_Tell(PyHP_Core_Stderr_Object *self)
{
    return PyLong_FromSsize_t(self->data.endpos);
}

static PyObject *PyHP_Core_Stderr_Write(PyHP_Core_Stderr_Object *self, PyObject *string)
{
    const char *raw;
    Py_ssize_t len;

    if (self->data.allocated == -1)
    {
        PyHP_Core_BaseStream_ClosedErr();
        return NULL;
    }

    if (!PyUnicode_Check(string))
    {
        PyErr_Format(
                PyExc_TypeError,
                "can write only strings"
        );
        return NULL;
    }

    raw = PyHP_Core_EncodeStringRO(string, &len);
    if (raw == NULL)
    {
        return NULL;
    }

    if (PyHP_Core_Stream_Write(&self->data, raw, 0, len) != 0)
    {
        return NULL;
    }

    return PyLong_FromSsize_t(len);
}

static PyMethodDef PyHP_Core_Stderr_Methods[] = {
        {"readline",   (PyCFunction) NULL,                        METH_VARARGS, ""},
        {"read",       (PyCFunction) NULL,                        METH_VARARGS, ""},
        {"close",      (PyCFunction) PyHP_Core_Stderr_Close,     METH_NOARGS,  ""},
        {"fileno",     (PyCFunction) PyHP_Core_Stderr_FileNo,    METH_NOARGS,  ""},
        {"flush",      (PyCFunction) PyHP_Core_Stderr_Flush,     METH_NOARGS,  ""},
        {"isatty",     (PyCFunction) PyHP_Core_Stderr_Isatty,    METH_NOARGS,  ""},
        {"readable",   (PyCFunction) PyHP_Core_Stderr_Readable,  METH_NOARGS,  ""},
        {"writeable",  (PyCFunction) PyHP_Core_Stderr_Writeable, METH_NOARGS,  ""},
        {"readlines",  (PyCFunction) NULL,                        METH_VARARGS, ""},
        {"seekable",   (PyCFunction) PyHP_Core_Stderr_Seekable,  METH_NOARGS,  ""},
        {"seek",       (PyCFunction) NULL,                        METH_VARARGS | METH_KEYWORDS, ""},
        {"tell",       (PyCFunction) PyHP_Core_Stderr_Tell,      METH_NOARGS,  ""},
        {"truncate",   (PyCFunction) NULL,                        METH_VARARGS, ""},
        {"write",      (PyCFunction) PyHP_Core_Stderr_Write,     METH_O,       ""},
        {"writelines", (PyCFunction) NULL,                        METH_O,       ""},
        {"detach",     (PyCFunction) NULL,                        METH_NOARGS,  ""},
        {NULL}
};

static PyObject *PyHP_Core_Stderr_GetClosed(PyHP_Core_Stderr_Object *self)
{
    if (self->data.pos == -1)
    {
        Py_RETURN_TRUE;
    }
    else
    {
        Py_RETURN_FALSE;
    }
}

static PyObject *PyHP_Core_Stderr_GetMode(PyHP_Core_Stderr_Object *self)
{
    return PyUnicode_FromString("wt");
}

static PyObject *PyHP_Core_Stderr_GetName(PyHP_Core_Stderr_Object *self)
{
    return PyUnicode_FromString("<stderr>");
}

static PyObject *PyHP_Core_Stderr_GetEncoding(PyHP_Core_Stderr_Object *self)
{
    return PyUnicode_FromString("utf-8");
}

static PyObject *PyHP_Core_Stderr_GetErrors(PyHP_Core_Stderr_Object *self)
{
    return PyUnicode_FromString("strict");
}

static PyObject *PyHP_Core_Stderr_GetNewlines(PyHP_Core_Stderr_Object *self)
{
    return PyUnicode_FromString("\n");
}

static PyObject *PyHP_Core_Stderr_GetBuffer(PyHP_Core_Stderr_Object *self)
{
    return PyBytes_FromStringAndSize(self->data.buffer, self->data.endpos);
}

static PyGetSetDef PyHP_Core_Stderr_GetSet[] = {
        {"closed",   (getter) PyHP_Core_Stderr_GetClosed,   NULL, ""},
        {"mode",     (getter) PyHP_Core_Stderr_GetMode,     NULL, ""},
        {"name",     (getter) PyHP_Core_Stderr_GetName,     NULL, ""},
        {"encoding", (getter) PyHP_Core_Stderr_GetEncoding, NULL, ""},
        {"errors",   (getter) PyHP_Core_Stderr_GetErrors,   NULL, ""},
        {"newlines", (getter) PyHP_Core_Stderr_GetNewlines, NULL, ""},
        {"buffer",   (getter) PyHP_Core_Stderr_GetBuffer,   NULL, ""},
        {NULL}
};

PyTypeObject PyHP_Core_Stderr_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.stderr",
        .tp_base = &PyHP_Core_BaseStream_Type,
        .tp_basicsize = sizeof(PyHP_Core_Stderr_Object),
        .tp_dealloc = (destructor) PyHP_Core_Stderr_Dealloc,
        .tp_methods = PyHP_Core_Stderr_Methods,
        .tp_getset = PyHP_Core_Stderr_GetSet,
};


static void PyHP_Core_StdoutPipe_Dealloc(PyHP_Core_StdoutPipe_Object *self)
{
    Py_DECREF(self->data);
    Py_TYPE(self)->tp_free(self);
}


static PyObject *PyHP_Core_StdoutPipe_Close(PyHP_Core_StdoutPipe_Object *self)
{
    self->data->data.allocated = -1;
    Py_RETURN_NONE;
}

static PyObject *PyHP_Core_StdoutPipe_FileNo(PyHP_Core_StdoutPipe_Object *self)
{
    return PyLong_FromLong(1);
}

static PyObject *PyHP_Core_StdoutPipe_Flush(PyHP_Core_StdoutPipe_Object *self)
{
    Py_RETURN_NONE;
}

static PyObject *PyHP_Core_StdoutPipe_Isatty(PyHP_Core_StdoutPipe_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_StdoutPipe_Readable(PyHP_Core_StdoutPipe_Object *self)
{
    Py_RETURN_TRUE;
}

static PyObject *PyHP_Core_StdoutPipe_Writeable(PyHP_Core_StdoutPipe_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_StdoutPipe_Seekable(PyHP_Core_StdoutPipe_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_StdoutPipe_Tell(PyHP_Core_StdoutPipe_Object *self)
{
    return PyLong_FromSsize_t(self->data->data.endpos);
}

static PyObject *PyHP_Core_StdoutPipe_Read(PyHP_Core_StdoutPipe_Object *self, PyObject *args)
{
    const char *raw;
    Py_ssize_t len;
    PyObject *slice;

    if (self->data->data.allocated == -1)
    {
        PyHP_Core_BaseStream_ClosedErr();
        return NULL;
    }

    if (PyTuple_Size(args) > 0)
    {
        if (!PyArg_ParseTuple(args, "|n", &len))
        {
            return NULL;
        }

        if (len < 0)
        {
            PyErr_Format(
                    PyExc_ValueError,
                    "can't read negative count of chars"
            );
            return NULL;
        }
        raw = PyHP_Core_Stream_ReadN(&(self->data->data), &len);
    }
    else
    {
        raw = PyHP_Core_Stream_Read(&(self->data->data), &len);
    }
    if (raw == NULL)
    {
        return NULL;
    }

    slice = PyHP_Core_DecodeString(raw, 0, len);
    if (slice == NULL)
    {
        return NULL;
    }

    return slice;
}

static PyMethodDef PyHP_Core_StdoutPipe_Methods[] = {
        {"readline",   (PyCFunction) NULL,                            METH_VARARGS, ""},
        {"read",       (PyCFunction) PyHP_Core_StdoutPipe_Read,      METH_VARARGS, ""},
        {"close",      (PyCFunction) PyHP_Core_StdoutPipe_Close,     METH_NOARGS,  ""},
        {"fileno",     (PyCFunction) PyHP_Core_StdoutPipe_FileNo,    METH_NOARGS,  ""},
        {"flush",      (PyCFunction) PyHP_Core_StdoutPipe_Flush,     METH_NOARGS,  ""},
        {"isatty",     (PyCFunction) PyHP_Core_StdoutPipe_Isatty,    METH_NOARGS,  ""},
        {"readable",   (PyCFunction) PyHP_Core_StdoutPipe_Readable,  METH_NOARGS,  ""},
        {"writeable",  (PyCFunction) PyHP_Core_StdoutPipe_Writeable, METH_NOARGS,  ""},
        {"readlines",  (PyCFunction) NULL,                            METH_VARARGS, ""},
        {"seekable",   (PyCFunction) PyHP_Core_StdoutPipe_Seekable,  METH_NOARGS,  ""},
        {"seek",       (PyCFunction) NULL,                            METH_VARARGS | METH_KEYWORDS, ""},
        {"tell",       (PyCFunction) PyHP_Core_StdoutPipe_Tell,      METH_NOARGS,  ""},
        {"truncate",   (PyCFunction) NULL,                            METH_VARARGS, ""},
        {"write",      (PyCFunction) NULL,                            METH_O,       ""},
        {"writelines", (PyCFunction) NULL,                            METH_O,       ""},
        {"detach",     (PyCFunction) NULL,                            METH_NOARGS,  ""},
        {NULL}
};

static PyObject *PyHP_Core_StdoutPipe_GetClosed(PyHP_Core_StdoutPipe_Object *self)
{
    if (self->data->data.pos == -1)
    {
        Py_RETURN_TRUE;
    }
    else
    {
        Py_RETURN_FALSE;
    }
}

static PyObject *PyHP_Core_StdoutPipe_GetMode(PyHP_Core_StdoutPipe_Object *self)
{
    return PyUnicode_FromString("rt");
}

static PyObject *PyHP_Core_StdoutPipe_GetName(PyHP_Core_StdoutPipe_Object *self)
{
    return PyUnicode_FromString("<stdout>");
}

static PyObject *PyHP_Core_StdoutPipe_GetEncoding(PyHP_Core_StdoutPipe_Object *self)
{
    return PyUnicode_FromString("utf-8");
}

static PyObject *PyHP_Core_StdoutPipe_GetErrors(PyHP_Core_StdoutPipe_Object *self)
{
    return PyUnicode_FromString("strict");
}

static PyObject *PyHP_Core_StdoutPipe_GetNewlines(PyHP_Core_StdoutPipe_Object *self)
{
    return PyUnicode_FromString("\n");
}

static PyObject *PyHP_Core_StdoutPipe_GetBuffer(PyHP_Core_StdoutPipe_Object *self)
{
    return PyBytes_FromStringAndSize(self->data->data.buffer, self->data->data.endpos);
}

static PyGetSetDef PyHP_Core_StdoutPipe_GetSet[] = {
        {"closed",   (getter) PyHP_Core_StdoutPipe_GetClosed,   NULL, ""},
        {"mode",     (getter) PyHP_Core_StdoutPipe_GetMode,     NULL, ""},
        {"name",     (getter) PyHP_Core_StdoutPipe_GetName,     NULL, ""},
        {"encoding", (getter) PyHP_Core_StdoutPipe_GetEncoding, NULL, ""},
        {"errors",   (getter) PyHP_Core_StdoutPipe_GetErrors,   NULL, ""},
        {"newlines", (getter) PyHP_Core_StdoutPipe_GetNewlines, NULL, ""},
        {"buffer",   (getter) PyHP_Core_StdoutPipe_GetBuffer,   NULL, ""},
        {NULL}
};


PyTypeObject PyHP_Core_StdoutPipe_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.stdout_pipe",
        .tp_base = &PyHP_Core_BaseStream_Type,
        .tp_basicsize = sizeof(PyHP_Core_StdoutPipe_Object),
        .tp_dealloc = (destructor) PyHP_Core_StdoutPipe_Dealloc,
        .tp_methods = PyHP_Core_StdoutPipe_Methods,
        .tp_getset = PyHP_Core_StdoutPipe_GetSet,
};

static void PyHP_Core_StderrPipe_Dealloc(PyHP_Core_StderrPipe_Object *self)
{
    Py_DECREF(self->data);
    Py_TYPE(self)->tp_free(self);
}


static PyObject *PyHP_Core_StderrPipe_Close(PyHP_Core_StderrPipe_Object *self)
{
    self->data->data.allocated = -1;
    Py_RETURN_NONE;
}

static PyObject *PyHP_Core_StderrPipe_FileNo(PyHP_Core_StderrPipe_Object *self)
{
    return PyLong_FromLong(2);
}

static PyObject *PyHP_Core_StderrPipe_Flush(PyHP_Core_StderrPipe_Object *self)
{
    Py_RETURN_NONE;
}

static PyObject *PyHP_Core_StderrPipe_Isatty(PyHP_Core_StderrPipe_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_StderrPipe_Readable(PyHP_Core_StderrPipe_Object *self)
{
    Py_RETURN_TRUE;
}

static PyObject *PyHP_Core_StderrPipe_Writeable(PyHP_Core_StderrPipe_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_StderrPipe_Seekable(PyHP_Core_StderrPipe_Object *self)
{
    Py_RETURN_FALSE;
}

static PyObject *PyHP_Core_StderrPipe_Tell(PyHP_Core_StderrPipe_Object *self)
{
    return PyLong_FromSsize_t(self->data->data.endpos);
}

static PyObject *PyHP_Core_StderrPipe_Read(PyHP_Core_StderrPipe_Object *self, PyObject *args)
{
    const char *raw;
    Py_ssize_t len;
    PyObject *slice;

    if (self->data->data.allocated == -1)
    {
        PyHP_Core_BaseStream_ClosedErr();
        return NULL;
    }

    if (PyTuple_Size(args) > 0)
    {
        if (!PyArg_ParseTuple(args, "|n", &len))
        {
            return NULL;
        }

        if (len < 0)
        {
            PyErr_Format(
                    PyExc_ValueError,
                    "can't read negative count of chars"
            );
            return NULL;
        }
        raw = PyHP_Core_Stream_ReadN(&(self->data->data), &len);
    }
    else
    {
        raw = PyHP_Core_Stream_Read(&(self->data->data), &len);
    }
    if (raw == NULL)
    {
        return NULL;
    }

    slice = PyHP_Core_DecodeString(raw, 0, len);
    if (slice == NULL)
    {
        return NULL;
    }

    return slice;
}


static PyMethodDef PyHP_Core_StderrPipe_Methods[] = {
        {"readline",   (PyCFunction) NULL,                            METH_VARARGS, ""},
        {"read",       (PyCFunction) PyHP_Core_StderrPipe_Read,      METH_VARARGS, ""},
        {"close",      (PyCFunction) PyHP_Core_StderrPipe_Close,     METH_NOARGS,  ""},
        {"fileno",     (PyCFunction) PyHP_Core_StderrPipe_FileNo,    METH_NOARGS,  ""},
        {"flush",      (PyCFunction) PyHP_Core_StderrPipe_Flush,     METH_NOARGS,  ""},
        {"isatty",     (PyCFunction) PyHP_Core_StderrPipe_Isatty,    METH_NOARGS,  ""},
        {"readable",   (PyCFunction) PyHP_Core_StderrPipe_Readable,  METH_NOARGS,  ""},
        {"writeable",  (PyCFunction) PyHP_Core_StderrPipe_Writeable, METH_NOARGS,  ""},
        {"readlines",  (PyCFunction) NULL,                            METH_VARARGS, ""},
        {"seekable",   (PyCFunction) PyHP_Core_StderrPipe_Seekable,  METH_NOARGS,  ""},
        {"seek",       (PyCFunction) NULL,                            METH_VARARGS | METH_KEYWORDS, ""},
        {"tell",       (PyCFunction) PyHP_Core_StderrPipe_Tell,      METH_NOARGS,  ""},
        {"truncate",   (PyCFunction) NULL,                            METH_VARARGS, ""},
        {"write",      (PyCFunction) NULL,                            METH_O,       ""},
        {"writelines", (PyCFunction) NULL,                            METH_O,       ""},
        {"detach",     (PyCFunction) NULL,                            METH_NOARGS,  ""},
        {NULL}
};

static PyObject *PyHP_Core_StderrPipe_GetClosed(PyHP_Core_StderrPipe_Object *self)
{
    if (self->data->data.pos == -1)
    {
        Py_RETURN_TRUE;
    }
    else
    {
        Py_RETURN_FALSE;
    }
}

static PyObject *PyHP_Core_StderrPipe_GetMode(PyHP_Core_StderrPipe_Object *self)
{
    return PyUnicode_FromString("rt");
}

static PyObject *PyHP_Core_StderrPipe_GetName(PyHP_Core_StderrPipe_Object *self)
{
    return PyUnicode_FromString("<stderr>");
}

static PyObject *PyHP_Core_StderrPipe_GetEncoding(PyHP_Core_StderrPipe_Object *self)
{
    return PyUnicode_FromString("utf-8");
}

static PyObject *PyHP_Core_StderrPipe_GetErrors(PyHP_Core_StderrPipe_Object *self)
{
    return PyUnicode_FromString("strict");
}

static PyObject *PyHP_Core_StderrPipe_GetNewlines(PyHP_Core_StderrPipe_Object *self)
{
    return PyUnicode_FromString("\n");
}

static PyObject *PyHP_Core_StderrPipe_GetBuffer(PyHP_Core_StderrPipe_Object *self)
{
    return PyBytes_FromStringAndSize(self->data->data.buffer, self->data->data.endpos);
}

static PyGetSetDef PyHP_Core_StderrPipe_GetSet[] = {
        {"closed",   (getter) PyHP_Core_StderrPipe_GetClosed,   NULL, ""},
        {"mode",     (getter) PyHP_Core_StderrPipe_GetMode,     NULL, ""},
        {"name",     (getter) PyHP_Core_StderrPipe_GetName,     NULL, ""},
        {"encoding", (getter) PyHP_Core_StderrPipe_GetEncoding, NULL, ""},
        {"errors",   (getter) PyHP_Core_StderrPipe_GetErrors,   NULL, ""},
        {"newlines", (getter) PyHP_Core_StderrPipe_GetNewlines, NULL, ""},
        {"buffer",   (getter) PyHP_Core_StderrPipe_GetBuffer,   NULL, ""},
        {NULL}
};

PyTypeObject PyHP_Core_StderrPipe_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.stderr_pipe",
        .tp_base = &PyHP_Core_BaseStream_Type,
        .tp_basicsize = sizeof(PyHP_Core_StderrPipe_Object),
        .tp_dealloc = (destructor) PyHP_Core_StderrPipe_Dealloc,
        .tp_methods = PyHP_Core_StderrPipe_Methods,
        .tp_getset = PyHP_Core_StderrPipe_GetSet
};


