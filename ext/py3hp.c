#include <python.h>
#include <structmember.h>

static PyObject * InputFunc(PyObject *__module__, PyObject *args)
{
    PyObject *prompt;

    if (!PyArg_ParseTuple(args, "|O", &prompt))
    {
        return NULL;
    }

    return PyUnicode_New(0, 0);
}

static PyMethodDef module_functions[] = {
     {"input", (PyCFunction)InputFunc, METH_VARARGS, ""},
     {NULL}
};

static struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "py3hp.ext",
    "",
    -1,
    module_functions
};

PyMODINIT_FUNC PyInit_ext( void ) {
    PyObject *module;

    module = PyModule_Create(&module_def);
    if (module == NULL)
    {
        return NULL;
    }


//    PyModule_AddObject(module, "any", (PyObject *)&NULL);

    return module;
}