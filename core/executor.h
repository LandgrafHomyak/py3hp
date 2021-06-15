#include <Python.h>
#include "py3hp.h"
#include "compiler.h"

#ifndef PY3HP_CORE_EXECUTOR_H
# define PY3HP_CORE_EXECUTOR_H

typedef struct
{
    PyThreadState *parent;
    PyThreadState *interpreter;
    Py3hp_Core_PageCode *code;
    PyObject *o_stdin;
    PyObject *o_stdout;
    PyObject *o_stderr;
    int exit_code;
} Py3hp_Core_Task;

typedef struct
{
    PyObject_HEAD
    Py3hp_Core_Task data;
} Py3hp_Core_Task_Object;

extern PyTypeObject Py3hp_Core_Task_Type;

PY3HP_LOW_API int Py3hp_Core_ExecEmbed(Py3hp_Core_PageCode *code, PyObject *globals);

PY3HP_LOW_API int Py3hp_Core_Exec(Py3hp_Core_Task *meta, PyObject *globals);

PY3HP_HIGH_API PyObject *Py3hp_Core_ExecEmbed_Func(PyObject *module, PyObject *raw_code);

PY3HP_HIGH_API PyObject * Py3hp_Core_Exec_Func(PyObject *module, PyObject *args, PyObject *kwargs);


#endif /* PY3HP_CORE_EXECUTOR_H */
