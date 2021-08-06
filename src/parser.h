#include <Python.h>

#include <PyHP.h>

#ifndef PyHP_Internal_PARSER_H
#define PyHP_Internal_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

PyHP_ParserIterator_Object *PyHP_Parser_Func(PyObject *module, PyObject *args);

#ifdef __cplusplus
}
#endif
#endif /* PyHP_Internal_PARSER_H */
