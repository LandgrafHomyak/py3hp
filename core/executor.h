#include <Python.h>
#include "pyhp.h"
#include "compiler.h"

#ifndef PyHP_CORE_EXECUTOR_H
# define PyHP_CORE_EXECUTOR_H

typedef struct
{
    PyThreadState *parent;
    PyThreadState *interpreter;
    PyHP_Core_PageCode *code;
    PyObject *o_stdin;
    PyObject *o_stdout;
    PyObject *o_stderr;
    int exit_code;
} PyHP_Core_Task;

typedef struct
{
    PyObject_HEAD
    PyHP_Core_Task data;
} PyHP_Core_Task_Object;

extern PyTypeObject PyHP_Core_Task_Type;

PyHP_LOW_API int PyHP_Core_ExecEmbed(PyHP_Core_PageCode *code, PyObject *globals);

PyHP_LOW_API int PyHP_Core_Exec(PyHP_Core_Task *meta, PyObject *globals);

PyHP_HIGH_API PyObject *PyHP_Core_ExecEmbed_Func(PyObject *module, PyObject *raw_code);

PyHP_HIGH_API PyObject * PyHP_Core_Exec_Func(PyObject *module, PyObject *args, PyObject *kwargs);


#endif /* PyHP_CORE_EXECUTOR_H */
