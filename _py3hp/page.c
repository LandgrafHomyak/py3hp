#include <Python.h>
#include <structmember.h>

#include "page.h"


static PyMethodDef AppInstance_Methods[] = {
        {"instance", (PyCFunction) NULL, METH_NOARGS | METH_STATIC, "get (and init, if not) instance of "}
};

static Py_hash_t Page_Hash(Page_Object *self)
{
    return (Py_hash_t) self;
}

static PyObject *Page_Richcompare(Page_Object *self, PyObject *other, int op)
{
    if (op == Py_EQ)
    {
        if (self == (Page_Object *) other)
        {
            Py_RETURN_TRUE;
        }
        else
        {
            Py_RETURN_FALSE;
        }
    }
    if (op == Py_NE)
    {
        if (self != (Page_Object *) other)
        {
            Py_RETURN_TRUE;
        }
        else
        {
            Py_RETURN_FALSE;
        }
    }
    else
    {
        Py_RETURN_NOTIMPLEMENTED;
    }
}

static void Page_Dealloc(Page_Object *self)
{
    if (!(self->flags.finished) && self->flags.running)
    {
        PyErr_WarnFormat(
                PyExc_Warning,
                -1,
                "Reference to running page %S was lost, it will be deallocated only when process will be finished; use .terminate() to kill it",
                self
        );
        Py_INCREF(self);
        self->flags.lost_reference = 1;
        return;
    }

    Py_DECREF(self->source);
    Py_XDECREF(self->url);
    Py_XDECREF(self->finish_callback);

    if (self->sub_interpreter != NULL)
    {
        Py_EndInterpreter(self->sub_interpreter);
    }

    Py_TYPE(self)->tp_free(self);
}

static PyMemberDef Page_Members[] = {
        {"callback", T_OBJECT, offsetof(Page_Object, finish_callback), 0, ""},
        {"source", T_OBJECT, offsetof(Page_Object, source), READONLY, ""},
        {"url", T_OBJECT, offsetof(Page_Object, source), READONLY, ""},
        {NULL}
};

static PyObject *Page_GetTimeout(Page_Object *self)
{
    PyObject *o;

    o = PyFloat_FromDouble(self->timeout);
    if (o == NULL)
    {
        return NULL;
    }
    return o;
}

static int Page_SetTimeout(Page_Object *self, PyObject *value)
{
    double new_value;
    if (self->flags.running)
    {
        PyErr_Format(
                PyExc_PermissionError,
                "can't update timeout while page is running"
        );
        return -1;
    }

    if (self->flags.finished)
    {
        PyErr_WarnFormat(
                PyExc_Warning,
                -1,
                "updating timeout after page was finished has no effect"
        );
    }

    new_value = PyFloat_AsDouble(value);
    if (new_value <= 0)
    {
        PyErr_Format(
                PyExc_ValueError,
                "timeout must be positive"
        );
        return -1;
    }

    self->timeout = new_value;

    return 0;
}

static PyGetSetDef Page_GetSet[] = {
        {"timeout", (getter) Page_GetTimeout, (setter) Page_SetTimeout, ""},
        {NULL}
};

PyTypeObject Page_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.Page",
        .tp_basicsize = sizeof(Page_Object),
        .tp_itemsize = 0,
        .tp_new = NULL,
        .tp_hash = (hashfunc) Page_Hash,
        .tp_richcompare = (richcmpfunc) Page_Richcompare,
        .tp_dealloc = (destructor) Page_Dealloc,
        .tp_members = Page_Members,
        .tp_getset = Page_GetSet,
//        .tp_methods = AppInstance_Methods
};