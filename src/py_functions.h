#include <Python.h>
#include <PyHP.h>

#ifndef PyHP_PY_FUNCTIONS_H
#define PyHP_PY_FUNCTIONS_H
#ifdef __cplusplus
extern "C" {
#endif
PyHP_ParserIterator_Object *PyHP_Parser_Func(PyObject *module, PyObject *args);

PyObject *PyHP_AlignCode_Func(PyObject *module, PyObject *args, PyObject *kwargs);

PyHP_PrepareState_Object *PyHP_Prepare_Func(PyObject *module, PyObject *args);

#ifdef __cplusplus
}
#endif
#endif /* PyHP_PY_FUNCTIONS_H */
