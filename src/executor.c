#include <windows.h>

#include <Python.h>
#include <PyHP.h>

int PyHP_ExecEmbed(PyHP_PageCode *code, PyObject *globals)
{
    PyHP_Task meta = {NULL};
    Py_ssize_t i;
    Py_ssize_t len;
    PyHP_PageCode_Cell cell;
    PyObject *string;
    PyObject *result;


    len = PyHP_PageCode_SLEN(code);
    for (i = 0; i < len; i++)
    {
        meta.o_stdout = PySys_GetObject("stdout");

        cell = PyHP_PageCode_STATEMENTS(code)[i];
        switch (cell.type)
        {
            case PyHP_PageCodeType_TEXT:
                string = PyHP_DecodeString(PyHP_PageCode_BUFFER(code), cell.value.raw.start, cell.value.raw.len);
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
            case PyHP_PageCodeType_EVAL:
            case PyHP_PageCodeType_EXEC:

                result = PyEval_EvalCode(cell.value.code, globals, globals);
                if (result == NULL)
                {
                    return 1;
                }
                if (cell.type == PyHP_PageCodeType_EVAL)
                {
                    PyObject_Print(result, stdout, Py_PRINT_RAW);
                }
                Py_DECREF(result);
                break;
        }
    }

    return 0;
}

int PyHP_Exec(PyHP_Task *meta, PyObject *globals)
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


    if (PyHP_ExecEmbed(meta->code, globals) != 0)
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


PyObject *PyHP_ExecEmbed_Func(PyObject *module, PyObject *raw_code)
{
    PyHP_PageCode *code;
    PyHP_PageCode icode = {NULL};
    const char *src;
    Py_ssize_t len;


    if (PyHP_ExecEmbed(code, PyEval_GetGlobals()) != 0)
    {
        goto err;
    }

    if (icode.name != NULL)
    {
        PyHP_ReleasePageCode(code);
        PyHP_ReleaseCompile((PyHP_PageCode_Cell *) (icode.statements), (char *) (icode.buffer));
    }

    Py_RETURN_NONE;
    err:
    if (icode.name != NULL)
    {
        PyHP_ReleasePageCode(code);
        PyHP_ReleaseCompile((PyHP_PageCode_Cell *) (icode.statements), (char *) (icode.buffer));
    }

    return NULL;

}

PyObject *PyHP_Exec_Func(PyObject *module, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"", "globals", "stdin", "stdout", "stderr", NULL};
    PyObject *source;
    PyObject *globals;
    PyHP_Task meta = {NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!OOO", kw_list, &PyHP_PageCode_Type, &source, &PyDict_Type, &globals, &(meta.o_stdin), &(meta.o_stdout), &(meta.o_stderr)))
    {
        return NULL;
    }

    meta.parent = PyThreadState_Get();
    meta.code = &(((PyHP_PageCode_Object *) source)->string);
    if (PyHP_Exec(&meta, globals) != 0)
    {
        return NULL;
    }

    Py_RETURN_NONE;

}

static void PyHP_Task_Dealloc(PyHP_Task_Object *self)
{
    /* PyHP_ReleaseTask(&(self->data)); */
    Py_TYPE(self)->tp_free(self);
}

PyTypeObject PyHP_Task_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.task",
        .tp_dealloc = (destructor) PyHP_Task_Dealloc,
};