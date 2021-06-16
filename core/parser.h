#include <Python.h>
#include "pyhp.h"

#ifndef PyHP_CORE_PARSER_H
# define PyHP_CORE_PARSER_H

PyHP_LOW_API Py_ssize_t PyHP_Core_AlignCode(char *dst, const char *src, Py_ssize_t start, Py_ssize_t len);

PyHP_HIGH_API PyObject *PyHP_Core_AlignCode_Func(PyObject *module, PyObject *string);


typedef enum
{
    PyHP_Core_StatementType_NONE = 0,
    PyHP_Core_StatementType_RAW = 1,
    PyHP_Core_StatementType_INLINE = 2,
    PyHP_Core_StatementType_BLOCK = 3,
    PyHP_Core_StatementType_INLINE3 = 4,
    PyHP_Core_StatementType_BLOCK3 = 5,
    PyHP_Core_StatementType_INLINE2 = 6,
    PyHP_Core_StatementType_BLOCK2 = 7,
    PyHP_Core_StatementType_INLINE1 = 8,
    PyHP_Core_StatementType_BLOCK1 = 9,
} PyHP_Core_StatementType;

typedef struct
{
    PyHP_Core_StatementType type;
    Py_ssize_t start;
    Py_ssize_t end;
} PyHP_Core_ParserMatch;

typedef struct
{
    Py_ssize_t pos;
    Py_ssize_t raw_start;
    Py_ssize_t raw_end;
    PyHP_Core_ParserMatch code_match;
    signed int index;
} PyHP_Core_ParserIteratorState;


PyHP_LOW_API void PyHP_Core_Parser_Init(const char *string, Py_ssize_t len, PyHP_Core_ParserIteratorState *state);

PyHP_LOW_API PyHP_Core_ParserMatch PyHP_Core_Parser_Next(const char *string, Py_ssize_t len, PyHP_Core_ParserIteratorState *state);


typedef struct
{
    PyObject_VAR_HEAD
    PyHP_Core_ParserIteratorState state;
    char buffer[1];
} PyHP_Core_ParserIterator_Object;

typedef struct
{
    PyObject_HEAD
    PyObject *value;
    PyHP_Core_ParserMatch meta;
} PyHP_Core_ParserMatch_Object;

PyHP_HIGH_API PyHP_Core_ParserIterator_Object *PyHP_Core_Parser_Func(PyObject *module, PyObject *string);

extern PyTypeObject PyHP_Core_ParserIterator_Type;
extern PyTypeObject PyHP_Core_ParserMatch_Type;

#endif /* PyHP_CORE_PARSER_H */
