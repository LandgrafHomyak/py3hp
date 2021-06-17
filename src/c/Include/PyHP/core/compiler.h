#include <Python.h>
#include <PyHP/api.h>


#ifndef PyHP_CORE_COMPILER_H
# define PyHP_CORE_COMPILER_H

typedef struct
{
    PyObject_VAR_HEAD
    PyHP_API_PageCode data;
} PyHP_Core_PageCode_Object;

typedef struct
{
    PyObject_HEAD
    PyHP_Core_PageCode_Object *data;
    Py_ssize_t pos;
} PyHP_Core_PageCodeIterator_Object;

PyHP_Core_PageCode_Object *PyHP_Core_Compile_Func(PyObject *module, PyObject *args, PyObject *kwargs);

extern PyTypeObject PyHP_Core_PageCode_Type;
extern PyTypeObject PyHP_Core_PageCodeIterator_Type;

#endif /* PyHP_CORE_COMPILER_H */
