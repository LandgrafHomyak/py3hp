#include <Python.h>

#include "statement_type.h"

#ifndef PyHP_PARSER_MATCH_H
#define PyHP_PARSER_MATCH_H

typedef struct PyHP_ParserMatch
{
    PyHP_StatementType type;
    Py_ssize_t start;
    Py_ssize_t end;
} PyHP_ParserMatch;

typedef struct PyHP_ParserMatch_Object
{
    PyObject_HEAD
    PyHP_ParserMatch data;
    PyObject *value;
} PyHP_ParserMatch_Object;

extern PyTypeObject PyHP_ParserMatch_Type;

int PyHP_ParserMatch_Converter(PyHP_ParserMatch_Object *src, PyHP_ParserMatch *dst);

#endif /* PyHP_PARSER_MATCH_H */
