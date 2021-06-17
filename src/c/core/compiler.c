#include <Python.h>

#include <PyHP/api.h>
#include <PyHP/core.h>

PyHP_Core_PageCode_Object *PyHP_Core_Compile_Func(PyObject *module, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"", "name", "optimize", NULL};
    PyObject *source;
    PyObject *name;
    int optimize;
    const char *src;
    Py_ssize_t len;
    Py_ssize_t slen;
    PyHP_API_PageCode_Cell *statements;
    Py_ssize_t blen;
    char *buffer;
    PyHP_Core_PageCode_Object *self;

    optimize = -1;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "UU|$i", kw_list, &source, &name, &optimize))
    {
        return NULL;
    }

    src = PyHP_API_EncodeStringRO(source, &len);
    if (src == NULL)
    {
        return NULL;
    }

    if (PyHP_API_Compile(src, len, &slen, &statements, &blen, &buffer, optimize) != 0)
    {
        return NULL;
    }

    self = (PyHP_Core_PageCode_Object *) (PyHP_Core_PageCode_Type.tp_alloc(&PyHP_Core_PageCode_Type, PyHP_API_PageCode_EXTRASIZE(slen, blen)));
    if (self == NULL)
    {
        PyHP_API_ReleaseCompile(statements, buffer);
        PyErr_NoMemory();
        return NULL;
    }

    PyHP_API_PageCode_SLEN(&(self->data)) = slen;
    PyHP_API_PageCode_BLEN(&(self->data)) = blen;

    memcpy(PyHP_API_PageCode_STATEMENTS(&(self->data)), statements, sizeof(PyHP_API_PageCode_Cell) * slen);
    memcpy((char *) (PyHP_API_PageCode_BUFFER(&(self->data))), buffer, sizeof(char) * blen);
    PyHP_API_ReleaseCompile(statements, buffer);

    Py_INCREF(name);
    self->data.name = name;

    return self;
}

static void PyHP_Core_PageCode_Dealloc(PyHP_Core_PageCode_Object *self)
{
    PyHP_API_ReleasePageCode(&self->data);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *PyHP_Core_PageCode_Repr(PyHP_Core_PageCode_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object '%U'>",
            Py_TYPE(self)->tp_name,
            self->data.name
    );
}

static PyHP_Core_PageCodeIterator_Object *PyHP_Core_PageCode_Iter(PyHP_Core_PageCode_Object *self)
{
    PyHP_Core_PageCodeIterator_Object *iterator;

    iterator = (PyHP_Core_PageCodeIterator_Object *) (PyHP_Core_PageCodeIterator_Type.tp_alloc(&PyHP_Core_PageCodeIterator_Type, 0));
    if (iterator == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    Py_INCREF(self);
    iterator->data = self;
    iterator->pos = 0;

    return iterator;
}

PyTypeObject PyHP_Core_PageCode_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.page_code",
        .tp_basicsize = PyHP_API_PageCode_BASESIZE + sizeof(PyObject),
        .tp_itemsize = 1,
        .tp_dealloc = (destructor) PyHP_Core_PageCode_Dealloc,
        .tp_repr = (reprfunc) PyHP_Core_PageCode_Repr,
        .tp_iter = (getiterfunc) PyHP_Core_PageCode_Iter,
};


static void PyHP_Core_PageCodeIterator_Dealloc(PyHP_Core_PageCodeIterator_Object *self)
{
    Py_DECREF(self->data);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *PyHP_Core_PageCodeIterator_Repr(PyHP_Core_PageCodeIterator_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object of %R>",
            Py_TYPE(self)->tp_name,
            self->data
    );
}

static PyHP_Core_PageCodeIterator_Object *PyHP_Core_PageCodeIterator_Iter(PyHP_Core_PageCodeIterator_Object *self)
{
    Py_INCREF(self);
    return self;
}

static PyObject *PyHP_Core_PageCodeIterator_Next(PyHP_Core_PageCodeIterator_Object *self)
{
    PyObject *tuple;
    PyObject *statement;
    PyHP_API_PageCode_Cell data;

    if (self->pos >= PyHP_API_PageCode_SLEN(&(self->data->data)))
    {
        return NULL;
    }


    data = PyHP_API_PageCode_STATEMENTS(&(self->data->data))[self->pos];

    if (data.type == PyHP_API_PageCodeType_TEXT)
    {
        statement = PyHP_API_DecodeString(PyHP_API_PageCode_BUFFER(&(self->data->data)), data.value.raw.start, data.value.raw.len);
        if (statement == NULL)
        {
            return NULL;
        }
    }
    else
    {
        statement = data.value.code;
        Py_INCREF(statement);
    }

    tuple = Py_BuildValue("iO", data.type, statement);
    Py_DECREF(statement);
    if (tuple == NULL)
    {
        return NULL;
    }
    self->pos++;
    return tuple;
}

PyTypeObject PyHP_Core_PageCodeIterator_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.page_code_iterator",
        .tp_basicsize = sizeof(PyHP_Core_PageCodeIterator_Object),
        .tp_dealloc = (destructor) PyHP_Core_PageCodeIterator_Dealloc,
        .tp_repr = (reprfunc) PyHP_Core_PageCodeIterator_Repr,
        .tp_iter = (getiterfunc) PyHP_Core_PageCodeIterator_Iter,
        .tp_iternext = (iternextfunc) PyHP_Core_PageCodeIterator_Next,
};
