#include <Python.h>
#include "iterator_meta.h"

#ifndef PyHP_PARSER_H
#define PyHP_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct PyHP_ParserState
{
    PyHP_IteratorHead head;
    Py_ssize_t pos;
    Py_ssize_t raw_start;
    Py_ssize_t raw_end;
    PyHP_ParserMatch code_match;
    signed int index;
    void *string;
    Py_ssize_t len;
} PyHP_ParserState;

typedef struct PyHP_ParserIterator_Object
{
    PyObject_HEAD
    PyHP_ParserState data;
} PyHP_ParserIterator_Object;

#if 0
typedef struct PyHP_ParserState_X
{
    PyHP_ParserState data;
    PyHP_Iterator_Free_FuncType finalize;
} PyHP_ParserState_X;
#endif

extern PyHP_IteratorMeta_Object PyHP_ParserIterator_Type;

int PyHP_Parser_FromString(PyHP_ParserState *self, const char *string, Py_ssize_t len);

int PyHP_Parser_FromObject(PyHP_ParserState *self, PyObject *string);

int PyHP_Parser_Next(PyHP_ParserState *self, PyHP_ParserMatch *dst);

void PyHP_Parser_Free(PyHP_ParserState *self);

int PyHP_Parser_Copy(PyHP_ParserState *self, PyHP_ParserState *dst);

#if 0
int PyHP_ParserState_Converter(PyObject *src, PyHP_ParserState_X *dst);

PyHP_ParserIterator_Object *PyHP_ParserIterator_FromString(const char *string, Py_ssize_t len);
#endif

PyHP_ParserIterator_Object *PyHP_ParserIterator_Wrap(PyHP_ParserState *src);

int PyHP_ParserIterator_Converter(PyObject *src, PyHP_ParserIterator_Object **dst);

#define PyHP_ParserIterator_DATA(OBJECT) (((PyHP_ParserIterator_Object *)(OBJECT))->data)


#ifdef __cplusplus
}
#endif

#endif /* PyHP_PARSER_H */
