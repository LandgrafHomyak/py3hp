#include <Python.h>
#include <PyHP/api.h>

int PyHP_Core_ExecEmbed(PyHP_API_PageCode *code, PyObject *globals)
{
    PyHP_API_Task meta = {NULL};
    Py_ssize_t i;
    Py_ssize_t len;
    PyHP_API_PageCode_Cell cell;
    PyObject *string;
    PyObject *result;


    len = PyHP_API_PageCode_SLEN(code);
    for (i = 0; i < len; i++)
    {
        meta.o_stdout = PySys_GetObject("stdout");

        cell = PyHP_API_PageCode_STATEMENTS(code)[i];
        switch (cell.type)
        {
            case PyHP_API_PageCodeType_TEXT:
                string = PyHP_API_DecodeString(PyHP_API_PageCode_BUFFER(code), cell.value.raw.start, cell.value.raw.len);
                if (string == NULL)
                {
                    return 1;
                }


                result = PyObject_CallMethod(meta.o_stdout, "write", "O", string);

                Py_DECREF(string);
                if (result == NULL)
                {
                    return 1;
                }
                Py_DECREF(result);

                break;
            case PyHP_API_PageCodeType_EVAL:
            case PyHP_API_PageCodeType_EXEC:

                result = PyEval_EvalCode(cell.value.code, globals, globals);
                if (result == NULL)
                {
                    return 1;
                }
                if (cell.type == PyHP_API_PageCodeType_EVAL)
                {
                    PyObject_Print(result, stdout, Py_PRINT_RAW);
                }
                Py_DECREF(result);
                break;
        }
    }

    return 0;
}

int PyHP_Core_Exec(PyHP_API_Task *meta, PyObject *globals)
{
    PyThreadState *interpreter;
    interpreter = Py_NewInterpreter();
    if (interpreter == NULL)
    {
        return 1;
    }
    meta->interpreter = interpreter;
    if (PyThreadState_Get() != interpreter)
    {
        PyThreadState_Swap(interpreter);
    }


    if (PySys_SetObject("stdin", meta->o_stdin) != 0)
    {
        goto err;
    }
    if (PySys_SetObject("stdout", meta->o_stdout) != 0)
    {
        goto err;
    }
    if (PySys_SetObject("stderr", meta->o_stdout) != 0)
    {
        goto err;
    }

    if (PyEval_GetGlobals() != NULL)
    {

        if (PyDict_Update(globals, PyEval_GetGlobals()) != 0)
        {
            goto err;
        }
    }
    if (PyDict_Merge(globals, PyEval_GetBuiltins(), 0) != 0)
    {
        goto err;
    }


    if (PyHP_Core_ExecEmbed(meta->code, globals) != 0)
    {
        goto err;
    }


    Py_EndInterpreter(interpreter);
    PyThreadState_Swap(meta->parent);
    return 0;
    err:
    Py_EndInterpreter(interpreter);
    PyThreadState_Swap(meta->parent);
    return 1;

}