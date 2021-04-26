#include <Python.h>
//#include "page.h"
//#include "_code.h"
#include "code/parser.h"
#include "code/class.h"
#include "code.h"

/*
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
}*/

static PyMethodDef module_functions[] = {
        {"compile", (PyCFunction) Py3hp_Compile_O, METH_VARARGS | METH_KEYWORDS, ""},
        {NULL}
};

static struct PyModuleDef module_def = {
        PyModuleDef_HEAD_INIT,
        "_py3hp",
        "",
        -1,
        module_functions
};

PyMODINIT_FUNC PyInit__py3hp(void)
{
    PyObject *module;

    module = PyModule_Create(&module_def);
    if (module == NULL)
    {
        return NULL;
    }

    if (Code_Init(module) == -1)
        return NULL;


    return module;
}