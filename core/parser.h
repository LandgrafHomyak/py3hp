#include <Python.h>
#include "py3hp.h"

#ifndef PY3HP_CORE_PARSER_H
# define PY3HP_CORE_PARSER_H

PY3HP_LOW_API Py_ssize_t Py3hp_Core_AlignCode(char *dst, const char *src, Py_ssize_t start, Py_ssize_t len);

PY3HP_HIGH_API PyObject *Py3hp_Core_AlignCode_Func(PyObject *module, PyObject *string);


typedef enum
{
    Py3hp_Core_StatementType_NONE = 0,
    Py3hp_Core_StatementType_RAW = 1,
    Py3hp_Core_StatementType_INLINE = 2,
    Py3hp_Core_StatementType_BLOCK = 3,
    Py3hp_Core_StatementType_INLINE3 = 4,
    Py3hp_Core_StatementType_BLOCK3 = 5,
    Py3hp_Core_StatementType_INLINE2 = 6,
    Py3hp_Core_StatementType_BLOCK2 = 7,
    Py3hp_Core_StatementType_INLINE1 = 8,
    Py3hp_Core_StatementType_BLOCK1 = 9,
} Py3hp_Core_StatementType;

typedef struct
{
    Py3hp_Core_StatementType type;
    Py_ssize_t start;
    Py_ssize_t end;
} Py3hp_Core_ParserMatch;

typedef struct
{
    Py_ssize_t pos;
    Py_ssize_t raw_start;
    Py_ssize_t raw_end;
    Py3hp_Core_ParserMatch code_match;
    signed int index;
} Py3hp_Core_ParserIteratorState;


PY3HP_LOW_API void Py3hp_Core_Parser_Init(const char *string, Py_ssize_t len, Py3hp_Core_ParserIteratorState *state);

PY3HP_LOW_API Py3hp_Core_ParserMatch Py3hp_Core_Parser_Next(const char *string, Py_ssize_t len, Py3hp_Core_ParserIteratorState *state);


typedef struct
{
    PyObject_VAR_HEAD
    Py3hp_Core_ParserIteratorState state;
    char buffer[1];
} Py3hp_Core_ParserIterator_Object;

typedef struct
{
    PyObject_HEAD
    PyObject *value;
    Py3hp_Core_ParserMatch meta;
} Py3hp_Core_ParserMatch_Object;

PY3HP_HIGH_API Py3hp_Core_ParserIterator_Object *Py3hp_Core_Parser_Func(PyObject *module, PyObject *string);

extern PyTypeObject Py3hp_Core_ParserIterator_Type;
extern PyTypeObject Py3hp_Core_ParserMatch_Type;

#endif /* PY3HP_CORE_PARSER_H */
