#include <Python.h>

#ifndef PYHP_PARSER_H
#define PYHP_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    PyHP_StatementType_NONE = 0,
    PyHP_StatementType_RAW = 1,
    PyHP_StatementType_INLINE = 2,
    PyHP_StatementType_BLOCK = 3,
    PyHP_StatementType_INLINE3 = 4,
    PyHP_StatementType_BLOCK3 = 5,
    PyHP_StatementType_INLINE2 = 6,
    PyHP_StatementType_BLOCK2 = 7,
    PyHP_StatementType_INLINE1 = 8,
    PyHP_StatementType_BLOCK1 = 9,
} PyHP_StatementType;

typedef struct
{
    PyHP_StatementType type;
    Py_ssize_t start;
    Py_ssize_t end;
} PyHP_ParserMatch;

typedef struct
{
    Py_ssize_t pos;
    Py_ssize_t raw_start;
    Py_ssize_t raw_end;
    PyHP_ParserMatch code_match;
    signed int index;
} PyHP_ParserIteratorState;


void PyHP_Parser_Init(PyHP_ParserIteratorState *state);

PyHP_ParserMatch PyHP_Parser_Next_String(PyHP_ParserIteratorState *state, const char *string, Py_ssize_t len);

PyHP_ParserMatch PyHP_Parser_Next_Object(PyHP_ParserIteratorState *state, PyObject *string);


#define PyHP_AlignCodeS_Ascii(DST, SRC, START, LEN) (PyHP_AlignCodeS((DST), (SRC), (START), (LEN), PyUnicode_1BYTE_KIND))
#define PyHP_AlignCodeS_Object(DST, SRC, START) (PyHP_AlignCodeS(PyUnicode_DATA(DST), PyUnicode_DATA(SRC), (START), PyUnicode_GET_LENGTH(SRC), PyUnicode_KIND(SRC)))

PyObject *PyHP_AlignCode(PyObject *string, Py_ssize_t start, Py_ssize_t len);

PyObject *PyHP_AlignCode_Object(PyObject *string, Py_ssize_t start, Py_ssize_t len);

PyObject *PyHP_AlignCode_Func(PyObject *module, PyObject *args, PyObject *kwargs);

typedef struct
{
    PyObject_HEAD
    PyHP_ParserIteratorState state;
    PyObject *string;
} PyHP_ParserIterator_Object;

typedef struct
{
    PyObject_HEAD
    PyObject *value;
    PyHP_ParserMatch meta;
} PyHP_ParserMatch_Object;

PyHP_ParserIterator_Object *PyHP_Parser_Func(PyObject *module, PyObject *string);

extern PyTypeObject PyHP_ParserIterator_Type;

extern PyTypeObject PyHP_ParserMatch_Type;
#ifdef __cplusplus
}
#endif
#endif /* PYHP_PARSER_H */
