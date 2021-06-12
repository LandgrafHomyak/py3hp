#include <Python.h>

#include "highlevel.h"
#include "manager.h"
#include "../code/class.h"

PyObject *Py3hp_GetDefaultPool_O(PyObject *__module__)
{
    return (PyObject *) Pool_Manager_GetDefaultPoolOrCreate();
}

PyObject *Py3hp_Exec_O(PyObject *__module__, Py3hpCode_Object *code)
{
    PyThreadState *si;
    PyThreadState *old;

    old = PyGILState_GetThisThreadState();
    si = Py_NewInterpreter();
    if (si == NULL)
    {
        PyThreadState_Swap(old);
        PyErr_Format(
                PyExc_RuntimeError,
                "can't create sub-interpreter"
                );
        return NULL;
    }


    Py_RETURN_NONE;
}

