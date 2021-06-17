#include <Python.h>
#include <PyHP/api.h>

#ifndef PyHP_CORE_PARSER_H
# define PyHP_CORE_PARSER_H

PyObject *PyHP_Core_AlignCode_Func(PyObject *module, PyObject *string);


typedef struct
{
    PyObject_VAR_HEAD
    PyHP_API_ParserIteratorState state;
    char buffer[1];
} PyHP_Core_ParserIterator_Object;

typedef struct
{
    PyObject_HEAD
    PyObject *value;
    PyHP_API_ParserMatch meta;
} PyHP_Core_ParserMatch_Object;

PyHP_Core_ParserIterator_Object *PyHP_Core_Parser_Func(PyObject *module, PyObject *string);

extern PyTypeObject PyHP_Core_ParserIterator_Type;
extern PyTypeObject PyHP_Core_ParserMatch_Type;

#endif /* PyHP_CORE_PARSER_H */
