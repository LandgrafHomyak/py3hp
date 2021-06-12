#include <Python.h>

#include <stdio.h>

#include "streams.h"
#include "encoding.h"
#include "debug.h"

static void Py3hpCore_OutStream_Dealloc(Py3hpCore_OutStream_Object *self)
{
    PyMem_Free(self->buffer);
    Py_TYPE(self)->tp_free(self);
}

Py_ssize_t Py3hpCore_OutStream_Write(Py3hpCore_OutStream_Object *self, char *string, Py_ssize_t len)
{
    if (self->pos + len > self->allocated)
    {
        if (self->pos + len > self->allocated * 2)
        {
            self->allocated += len;
        }
        else
        {
            self->allocated *= 2;
        }

        self->buffer = PyMem_Realloc(self->buffer, self->allocated);

        if (self->buffer == NULL)
        {
            PyErr_NoMemory();
            return -1;
        }
    }

    if (len > 0)
    {
        memcpy(&(self->buffer[self->pos]), string, len);
        self->pos += len;
        if (self->pos > self->len)
        {
            self->len = self->pos;
        }
    }

    return len;
}

static PyObject *Py3hpCore_OutStream_Write_O(Py3hpCore_OutStream_Object *self, PyObject *string)
{
    PyObject *bytes;
    PyObject *written;
    Py_ssize_t r_len;
    Py_ssize_t w_len;

    if (!PyUnicode_Check(string))
    {
        PyErr_Format(
                PyExc_TypeError,
                "can write only strings into the py3hp page"
        );
        return NULL;
    }

    bytes = Py3hpCore_EncodeFromString_O(string);
    if (bytes == NULL)
    {
        return NULL;
    }

    r_len = PyBytes_GET_SIZE(bytes);
    w_len = Py3hpCore_OutStream_Write(self, PyBytes_AS_STRING(bytes), r_len);
    if (w_len != r_len)
    {
        return NULL;
    }

    written = PyLong_FromSsize_t(w_len);
    if (written == NULL)
    {
        return NULL;
    }

    return written;

}

static PyObject *Py3hpCore_OutStream_Flush(Py3hpCore_OutStream_Object *self)
{
    Py_RETURN_NONE;
}

static PyObject *Py3hpCore_OutStream_Close(Py3hpCore_OutStream_Object *self)
{
    Py_RETURN_NONE;
}

#ifdef Py3hpCore_Debug

static PyObject *Py3hpCore_OutStream_Read_O(Py3hpCore_OutStream_Object *self, PyObject *args)
{
    PyObject *string;
    PyObject *requested_len_o = NULL;
    Py_ssize_t requested_len;


    if (!PyArg_ParseTuple(args, "|O!", &PyLong_Type, &requested_len_o))
    {
        return NULL;
    }

    if (requested_len_o == NULL)
    {
        requested_len = self->len - self->pos;
    }
    else
    {
        requested_len = PyLong_AsSsize_t(requested_len_o);
        if (requested_len == -1 && PyErr_Occurred())
        {
            return NULL;
        }

        if (requested_len < 0)
        {
            requested_len = 0;
        }
        else if (requested_len > self->len - self->pos)
        {
            requested_len = self->len - self->pos;
        }
    }

    string = Py3hpCore_DecodeToString(&(self->buffer[self->pos]), requested_len);
    if (string == NULL)
    {
        return NULL;
    }

    self->pos += requested_len;

    return string;
}

static PyObject *Py3hpCore_OutStream_Seek(Py3hpCore_OutStream_Object *self, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"offset", "whence", NULL};
    Py_ssize_t abs_pos;
    PyObject *abs_pos_o;
    Py_ssize_t new_pos;
    Py_ssize_t whence = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "n|n", kw_list, &new_pos, &whence))
    {
        return NULL;
    }

    switch (whence)
    {
        case SEEK_SET:
            abs_pos = new_pos;
            break;
        case SEEK_CUR:
            abs_pos = self->pos + new_pos;
            break;
        case SEEK_END:
            abs_pos = self->len + new_pos;
            break;
        default:
            PyErr_Format(
                    PyExc_ValueError,
                    "Invalid whence (%zd, should be 0, 1 or 2)",
                    whence
            );
            return NULL;
    }

    if (abs_pos > self->allocated)
    {
        self->buffer = PyMem_Realloc(self->buffer, abs_pos);/* todo resizing */
        self->allocated = abs_pos;
        if (self->buffer == NULL)
        {
            PyErr_NoMemory();
            return NULL;
        }
    }

    self->pos = abs_pos;

    abs_pos_o = PyLong_FromSsize_t(abs_pos);
    if (abs_pos_o == NULL)
    {
        return NULL;
    }

    return abs_pos_o;

}

#endif

static PyMethodDef Py3hpCore_OutStream_Method[] = {
        {"write", (PyCFunction) Py3hpCore_OutStream_Write_O, METH_O, ""},
        {"flush", (PyCFunction) Py3hpCore_OutStream_Flush, METH_NOARGS, ""},
        {"close", (PyCFunction) Py3hpCore_OutStream_Close, METH_NOARGS, ""},
#ifdef Py3hpCore_Debug
        {"read", (PyCFunction) Py3hpCore_OutStream_Read_O, METH_VARARGS, ""},
        {"seek", (PyCFunction) Py3hpCore_OutStream_Seek, METH_VARARGS | METH_KEYWORDS, ""},
#endif
        {NULL}
};

PyTypeObject Py3hpCore_PageStdout_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.page_stdout",
        .tp_doc = "",
        .tp_basicsize = sizeof(Py3hpCore_OutStream_Object),
        .tp_itemsize = 0,
        .tp_new = NULL,
        .tp_dealloc = (destructor) Py3hpCore_OutStream_Dealloc,
        .tp_methods = Py3hpCore_OutStream_Method
};

PyTypeObject Py3hpCore_PageStderr_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.page_stderr",
        .tp_doc = "",
        .tp_basicsize = sizeof(Py3hpCore_OutStream_Object),
        .tp_itemsize = 0,
        .tp_new = NULL,
        .tp_dealloc = (destructor) Py3hpCore_OutStream_Dealloc,
        .tp_methods = Py3hpCore_OutStream_Method
};

Py3hpCore_OutStream_Object *Py3hpCore_OutStream_Create(PyTypeObject *cls)
{
    Py3hpCore_OutStream_Object *self;

    self = (Py3hpCore_OutStream_Object *) cls->tp_alloc(cls, 0);
    if (cls == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    self->len = 0;
    self->buffer = NULL;
    self->allocated = 0;

    return self;
}