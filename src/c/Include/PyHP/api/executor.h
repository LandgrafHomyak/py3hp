#include <Python.h>
#include "compiler.h"

#ifndef PyHP_API_EXECUTOR_H
# define PyHP_API_EXECUTOR_H

typedef struct
{
    PyThreadState *parent;
    PyThreadState *interpreter;
    PyHP_API_PageCode *code;
    PyObject *o_stdin;
    PyObject *o_stdout;
    PyObject *o_stderr;
    int exit_code;
} PyHP_API_Task;

int PyHP_API_ExecEmbed(PyHP_API_PageCode *code, PyObject *globals);

int PyHP_API_Exec(PyHP_API_Task *meta, PyObject *globals);


#endif /* PyHP_API_EXECUTOR_H */
