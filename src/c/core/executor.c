#include <Python.h>
#include <PyHP/api.h>
#include <PyHP/core.h>

PyObject *PyHP_Core_ExecEmbed_Func(PyObject *module, PyObject *raw_code)
{
    PyHP_API_PageCode *code;
    PyHP_API_PageCode icode = {NULL};
    const char *src;
    Py_ssize_t len;

    if (!(Py_TYPE(raw_code) == &PyHP_Core_PageCode_Type))
    {
        if (!(PyUnicode_Check(raw_code)))
        {
            PyErr_Format(
                    PyExc_TypeError,
                    "Can compile only strings"
            );
            return NULL;
        }

        src = PyHP_API_EncodeStringRO(raw_code, &len);
        if (src == NULL)
        {
            return NULL;

        }

        icode.name = PyUnicode_FromString("<py3hp page>");
        if (icode.name == NULL)
        {
            return NULL;

        }

        code = &icode;
        if (PyHP_API_Compile(src, len, &(PyHP_API_PageCode_SLEN(code)), (PyHP_API_PageCode_Cell **) &(icode.statements), &(PyHP_API_PageCode_BLEN(code)), (char **) &(icode.buffer), -1) != 0)
        {
            Py_DECREF(icode.name);
            return NULL;

        }
    }
    else
    {
        code = &((PyHP_Core_PageCode_Object *) raw_code)->data;
    }

    if (PyHP_API_ExecEmbed(code, PyEval_GetGlobals()) != 0)
    {
        goto err;
    }

    if (icode.name != NULL)
    {
        PyHP_API_ReleasePageCode(code);
        PyHP_API_ReleaseCompile((PyHP_API_PageCode_Cell *) (icode.statements), (char *) (icode.buffer));
    }

    Py_RETURN_NONE;
    err:
    if (icode.name != NULL)
    {
        PyHP_API_ReleasePageCode(code);
        PyHP_API_ReleaseCompile((PyHP_API_PageCode_Cell *) (icode.statements), (char *) (icode.buffer));
    }

    return NULL;

}

PyObject *PyHP_Core_Exec_Func(PyObject *module, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"", "globals", "stdin", "stdout", "stderr", NULL};
    PyObject *source;
    PyObject *globals;
    PyHP_API_Task meta = {NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!OOO", kw_list, &PyHP_Core_PageCode_Type, &source, &PyDict_Type, &globals, &(meta.o_stdin), &(meta.o_stdout), &(meta.o_stderr)))
    {
        return NULL;
    }

    meta.parent = PyThreadState_Get();
    meta.code = &(((PyHP_Core_PageCode_Object *) source)->data);
    if (PyHP_API_Exec(&meta, globals) != 0)
    {
        return NULL;
    }

    Py_RETURN_NONE;

}

static void PyHP_Core_Task_Dealloc(PyHP_Core_Task_Object *self)
{
    /* PyHP_API_ReleaseTask(&(self->data)); */
    Py_TYPE(self)->tp_free(self);
}

PyTypeObject PyHP_Core_Task_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.task",
        .tp_dealloc = (destructor) PyHP_Core_Task_Dealloc,
};