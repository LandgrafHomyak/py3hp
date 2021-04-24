#include <Python.h>
#include "page.h"
#include "code.h"

static PyObject *pages_pool = NULL;

static PyObject *Interpret(PyObject *module, PyObject *args, PyObject *kwargs)
{
    static char *kw_names[] = {"", "url", "callback", "timeout", NULL};
    PyObject *source;
    PyObject *url = NULL;
    PyObject *callback = NULL;
    double timeout = 2;
    Page_Object *self;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "U|$UOd", kw_names, &source, &url, &callback, &timeout))
    {
        return NULL;
    }
    if (timeout <= 0)
    {
        PyErr_Format(
                PyExc_ValueError,
                "timeout must be positive"
        );
        return NULL;
    }

    self = (Page_Object *) Page_Type.tp_alloc(&Page_Type, 0);
    if (self == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }


    Py_INCREF(source);
    self->source = source;
    Py_XINCREF(url);
    self->url = url;
    Py_XINCREF(callback);
    self->finish_callback = callback;
    self->timeout = timeout;
    self->sub_interpreter = NULL;
    self->flags.running = 0;
    self->flags.finished = 0;
    self->flags.lost_reference = 0;

    if (PySet_Add(pages_pool, self) != 0)
    {
        Py_DECREF(self);
        return NULL;
    }

    return self;
}

static PyMethodDef module_functions[] = {
        {"interpret", (PyCFunction) Interpret,    METH_VARARGS | METH_KEYWORDS, ""},
        {"compile",   (PyCFunction) Code_Compile, METH_O, ""},
        {NULL}
};

static struct PyModuleDef module_def = {
        PyModuleDef_HEAD_INIT,
        "py3hp",
        "",
        -1,
        module_functions
};

PyMODINIT_FUNC PyInit__py3hp(void)
{
    PyObject *module;

    module = PyImport_ImportModule("io");
    if (module == NULL)
    {
        return NULL;
    }
    StringIO_Type = (PyTypeObject *) PyObject_GetAttrString(module, "StringIO");
    Py_DECREF(module);
    if (StringIO_Type == NULL)
    {
        return NULL;
    }

    module = PyModule_Create(&module_def);
    if (module == NULL)
    {
        return NULL;
    }
    if (PyType_Ready(&Page_Type))
    {
        Py_DECREF(module);
        return NULL;
    }

    pages_pool = PySet_New(NULL);
    if (pages_pool == NULL)
    {
        Py_DECREF(module);
        return NULL;
    }



//    PyModule_AddObject(module, "any", (PyObject *)&NULL);

    return module;
}