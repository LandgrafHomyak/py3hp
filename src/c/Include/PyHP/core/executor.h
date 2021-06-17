#include <Python.h>
#include <PyHP/api.h>

#ifndef PyHP_CORE_EXECUTOR_H
# define PyHP_CORE_EXECUTOR_H

typedef struct
{
    PyObject_HEAD
    PyHP_API_Task data;
} PyHP_Core_Task_Object;

extern PyTypeObject PyHP_Core_Task_Type;

PyObject *PyHP_Core_ExecEmbed_Func(PyObject *module, PyObject *raw_code);

PyObject *PyHP_Core_Exec_Func(PyObject *module, PyObject *args, PyObject *kwargs);


#endif /* PyHP_CORE_EXECUTOR_H */
