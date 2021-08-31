#include <Python.h>
#include <PyHP.h>

#include "py_functions.h"

int PyHP_ExecEmbed(PyHP_CompilerStateWithParent *code_iterator, PyObject *out_file, PyObject *globals)
{
    PyObject *orig_stdout;
    PyHP_Command command;
    int ret;
    PyObject *res_val1;
    PyObject *res_val2;

    orig_stdout = PySys_GetObject("stdout");
    if (orig_stdout == NULL)
    {
        return -1;
    }
    if (PySys_SetObject("stdout", out_file) != 0)
    {
        return -1;
    }

    ret = -1;
    while (1)
    {
        switch (PyHP_Compiler_Next(code_iterator, &command))
        {
            case PyHP_Iterator_NEXT_ERROR:
                goto restore;
            case PyHP_Iterator_NEXT_END:
                break;
            case PyHP_Iterator_NEXT_SUCCESSFUL:
                switch (command.type)
                {
                    case PyHP_CommandType_TEXT:
                        res_val1 = PyObject_CallMethod(out_file, "write", "O", command.value);
                        if (res_val1 == NULL)
                        {
                            goto restore;
                        }
                        Py_DECREF(res_val1);
                        break;
                    case PyHP_CommandType_EVAL:
                        res_val1 = PyEval_EvalCode(command.value, globals, globals);
                        if (res_val1 == NULL)
                        {
                            goto restore;
                        }
                        res_val2 = PyObject_Str(res_val1);
                        Py_DECREF(res_val1);
                        if (res_val2 == NULL)
                        {
                            goto restore;
                        }
                        res_val1 = PyObject_CallMethod(out_file, "write", "O", res_val2);
                        Py_DECREF(res_val2);
                        if (res_val1 == NULL)
                        {
                            goto restore;
                        }
                        Py_DECREF(res_val1);
                        break;
                    case PyHP_CommandType_EXEC:
                        res_val1 = PyEval_EvalCode(command.value, globals, globals);
                        if (res_val1 == NULL)
                        {
                            goto restore;
                        }
                        Py_DECREF(res_val1);
                        break;
                }
                continue;
        }
        break;
    }

    ret = 0;
    restore:
    if (PySys_SetObject("stdout", orig_stdout) != 0)
    {
        return -1;
    }

    return ret;

}

#if 0
int PyHP_Exec(PyHP_CompilerState *code_iterator, PyObject *out_file, PyObject *globals)
{
    PyThreadState *interpreter;
    interpreter = Py_NewInterpreter();
    if (interpreter == NULL)
    {
        return 1;
    }
    if (PyThreadState_Get() != interpreter)
    {
        PyThreadState_Swap(interpreter);
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
#endif

int PyHP_CompilerOrString_Converter(PyObject *src, PyHP_CompilerState_Object **dst)
{
    PyHP_ParserState p;
    PyHP_PrepareStateWithParent pp;
    PyHP_CompilerStateWithParent cp;

#if PY_VERSION_HEX >= 0x03010000
    if (src == NULL)
    {
        Py_DECREF(*dst);
        return 1;
    }
#endif

    if (Py_TYPE(src) == (PyTypeObject *) &PyHP_CompilerIterator_Type)
    {
        Py_INCREF(src);
        *dst = (PyHP_CompilerState_Object *) src;
#if PY_VERSION_HEX >= 0x03010000
        return Py_CLEANUP_SUPPORTED;
#else
        return 1;
#endif
    }
    else if (PyUnicode_CheckExact(src))
    {
        if (PyHP_Parser_FromObject(&p, src) != 0)
        {
            return 0;
        }
        if (PyHP_Prepare_WrapParser(&pp, &p) != 0)
        {
            PyHP_Parser_Free(&p);
            return 0;
        }
        if (PyHP_Compiler_WrapPrepare(&cp, &pp) != 0)
        {
            PyHP_Prepare_Free(&pp);
            return 0;
        }
        *dst = PyHP_CompilerIterator_Wrap(&cp);
        if (*dst == NULL)
        {
            PyHP_Compiler_Free(&cp);
            return 0;
        }
#if PY_VERSION_HEX >= 0x03010000
        return Py_CLEANUP_SUPPORTED;
#else
        return 1;
#endif
    }
    else
    {
        PyErr_Format(
            PyExc_TypeError,
            "Can't compile '%R'",
            src
        );
        return 0;
    }
}

PyObject *PyHP_ExecEmbed_Func(PyObject *module, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"", "stdout", "globals", NULL};
    PyHP_CompilerState_Object *code;
    PyObject *out_file;
    PyObject *globals = NULL;
    PyObject *orig_globals;


    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&O|O!", kw_list, PyHP_CompilerOrString_Converter, &code, &out_file, &PyDict_Type, &globals))
    {
        return NULL;
    }

    orig_globals = PyEval_GetGlobals();
    if (orig_globals == NULL)
    {
        if (PyErr_Occurred())
        {

            Py_DECREF(code);
            return NULL;
        }
        PyErr_WarnFormat(
            PyExc_RuntimeWarning,
            -1,
            "'exec_embed' invoked without python stack frame"
        );
    }

    if (globals == NULL)
    {
        if (orig_globals == NULL)
        {
            globals = PyDict_New();
            if (globals == NULL)
            {
                Py_DECREF(code);
                return NULL;
            }
        }
        else
        {
            globals = orig_globals;
            Py_INCREF(globals);
        }
    }
    else
    {
        if (orig_globals != NULL)
        {
            if (PyDict_Merge(globals, orig_globals, 0) != 0)
            {
                Py_DECREF(code);
                return NULL;
            }
        }
        Py_INCREF(globals);
    }

    if (PyHP_ExecEmbed(&PyHP_CompilerIterator_DATA(code), out_file, globals) != 0)
    {
        Py_DECREF(globals);
        Py_DECREF(code);
        return NULL;
    }

    Py_DECREF(globals);
    Py_DECREF(code);
    Py_RETURN_NONE;
}

#if 0
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
#endif