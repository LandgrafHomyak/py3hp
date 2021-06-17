#include <Python.h>

#ifndef PyHP_API_PARSER_H
# define PyHP_API_PARSER_H

typedef enum
{
    PyHP_API_StatementType_NONE = 0,
    PyHP_API_StatementType_RAW = 1,
    PyHP_API_StatementType_INLINE = 2,
    PyHP_API_StatementType_BLOCK = 3,
    PyHP_API_StatementType_INLINE3 = 4,
    PyHP_API_StatementType_BLOCK3 = 5,
    PyHP_API_StatementType_INLINE2 = 6,
    PyHP_API_StatementType_BLOCK2 = 7,
    PyHP_API_StatementType_INLINE1 = 8,
    PyHP_API_StatementType_BLOCK1 = 9,
} PyHP_API_StatementType;

typedef struct
{
    PyHP_API_StatementType type;
    Py_ssize_t start;
    Py_ssize_t end;
} PyHP_API_ParserMatch;

typedef struct
{
    Py_ssize_t pos;
    Py_ssize_t raw_start;
    Py_ssize_t raw_end;
    PyHP_API_ParserMatch code_match;
    signed int index;
} PyHP_API_ParserIteratorState;

void PyHP_API_Parser_Init(const char *string, Py_ssize_t len, PyHP_API_ParserIteratorState *state);

PyHP_API_ParserMatch PyHP_API_Parser_Next(const char *string, Py_ssize_t len, PyHP_API_ParserIteratorState *state);

Py_ssize_t PyHP_API_AlignCode(char *dst, const char *src, const Py_ssize_t start, const Py_ssize_t len);

#endif /* PyHP_API_PARSER_H */
