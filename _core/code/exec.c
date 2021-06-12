#include <Python.h>
#include "exec.h"
#include "class.h"
#include "commandtype.h"

int Code_Exec_TargetExec(Py3hpCode_Object *code, PyObject *stream)
{
    Py_ssize_t i;
    PyObject *bytes;
    PyObject *string;
    PyObject *value;
    PyObject *ret;

    for (i = 0; i < Py_SIZE(code); i++)
    {
        switch (code->commands[i].type)
        {
            case Py3hpCode_Command_RAW:
                bytes = PyBytes_FromStringAndSize(code->commands[i].pointer.raw, code->commands[i].len);
                if (bytes == NULL)
                {
                    return -1;
                }
                if (Py_TYPE(code) == &Py3hpCodeStr_Type)
                {
                    string = PyUnicode_FromEncodedObject(bytes, "utf-8", NULL);
                    Py_DECREF(bytes);
                    if (string == NULL)
                    {
                        return NULL;
                    }
                }
                else
                {
                    value = bytes;
                }
                break;
            case Py3hpCode_Command_EVAL:
            case Py3hpCode_Command_EXEC:
                value = PyEval_EvalCode(code->commands[i].pointer.code, NULL, NULL);
                if (value == NULL)
                {
                    return NULL;
                }
                if (code->commands[i].type == Py3hpCode_Command_EXEC)
                {
                    Py_DECREF(value);
                    value = NULL;
                }
                break;
        }
        if (code == NULL)
        {
            continue;
        }

        ret = PyObject_CallMethod(stream, "write", "O", value);
        Py_DECREF(value);
        if (ret == NULL)
        {
            return -1;
        }
        Py_DECREF(ret);
    }
    return 0;
}

PyThreadState *Code_Exec_SetupInterpreter(void)
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

    return si;
}

void Code_Exec_FinalizeInterpreter(PyThreadState *old,  PyThreadState *current)
{
    Py_EndInterpreter(current);
    PyThreadState_Swap(old);

}
