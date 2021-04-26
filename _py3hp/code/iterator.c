#include <Python.h>
#include "iterator.h"
#include "class.h"
#include "commandtype.h"

typedef struct
{
    PyObject_HEAD
    Py3hpCode_Object *master;
    Py_ssize_t i;
} Py3hpCode_Iterator_Object;


static void Py3hpCode_Iterator_Dealloc(Py3hpCode_Iterator_Object *self)
{
    Py_DECREF(self->master);
    Py_TYPE(self)->tp_free(self);
}

static Py3hpCode_Iterator_Object *Py3hpCode_Iterator_Iter(Py3hpCode_Iterator_Object *self)
{
    Py_INCREF(self);
    return self;
}

static PyObject *Py3hpCode_Iterator_Next(Py3hpCode_Iterator_Object *self)
{
    PyObject *tuple;
    PyObject *bytes;
    PyObject *string;
    PyObject *elem;

    if (self->i >= Py_SIZE(self->master))
    {
        return NULL;
    }

    switch (self->master->commands[self->i].type)
    {
        case Py3hpCode_Command_RAW:
            bytes = PyBytes_FromStringAndSize(self->master->commands[self->i].pointer.raw, self->master->commands[self->i].len);
            if (bytes == NULL)
            {
                return NULL;
            }

            if (Py_TYPE(self->master) == &Py3hpCodeStr_Type)
            {
                string = PyUnicode_FromEncodedObject(bytes, "utf-8", NULL);
                Py_DECREF(bytes);
                if (string == NULL)
                {
                    return NULL;
                }
                elem = string;
            }
            else
            {
                elem = bytes;
            }
            break;
        case Py3hpCode_Command_EVAL:
        case Py3hpCode_Command_EXEC:
            elem = self->master->commands[self->i].pointer.code;
            Py_INCREF(elem);
            break;
        default:
            PyErr_Format(
                    PyExc_RuntimeError,
                    "Unknown type of command no. %zd in %R (%d)",
                    self->i,
                    self->master,
                    self->master->commands[self->i].type
            );
            break;
    }

    tuple = Py_BuildValue("(OO)", elem, Py3hpCode_CommandType_Get(self->master->commands[self->i].type));
    Py_DECREF(elem);
    if (tuple == NULL)
    {
        return NULL;
    }

    self->i++;

    return tuple;
}

static PyObject *Py3hpCode_Iterator_Repr(Py3hpCode_Iterator_Object *self)
{
    return PyUnicode_FromFormat(
            "<py3hp_code iterator of %R (%zd/%zd) at %p>",
            self->master,
            self->i,
            Py_SIZE(self->master),
            self
    );
}


static PyTypeObject Py3hpCode_Iterator_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.py3hp_code_iterator",
        .tp_doc = "",
        .tp_basicsize = sizeof(Py3hpCode_Iterator_Object),
        .tp_itemsize = 0,
        .tp_new = NULL,
        .tp_dealloc = (destructor) Py3hpCode_Iterator_Dealloc,
        .tp_iter = (getiterfunc) Py3hpCode_Iterator_Iter,
        .tp_iternext = (iternextfunc) Py3hpCode_Iterator_Next,
        .tp_repr = (reprfunc) Py3hpCode_Iterator_Repr
};

PyObject *Py3hpCode_Iter(Py3hpCode_Object *master)
{
    Py3hpCode_Iterator_Object *self;

    self = (Py3hpCode_Iterator_Object *) Py3hpCode_Iterator_Type.tp_alloc(&Py3hpCode_Iterator_Type, 0);
    if (self == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    Py_INCREF(master);
    self->master = master;
    self->i = 0;

    return (PyObject *) self;
}


int Code_Iterator_Init(PyObject *module)
{
    if (PyType_Ready(&Py3hpCode_Iterator_Type))
    {
        return -1;
    }

    return 0;
}

void Code_Iterator_Free(void)
{

}