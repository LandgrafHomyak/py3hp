#include <Python.h>
#include "parser.h"

#ifndef PyHP_API_COMPILER_H
# define PyHP_API_COMPILER_H

typedef enum
{
    PyHP_API_PageCodeType_TEXT = 1,
    PyHP_API_PageCodeType_EXEC = 2,
    PyHP_API_PageCodeType_EVAL = 3,
} PyHP_API_PageCodeType;


typedef struct
{
    PyHP_API_PageCodeType type;
    union
    {
        PyObject *code;
        struct
        {
            Py_ssize_t start;
            Py_ssize_t len;
        } raw;
    } value;
} PyHP_API_PageCode_Cell;

int PyHP_API_CompileCell(PyHP_API_PageCode_Cell *dst, int py_version, PyHP_API_StatementType type, const char *source, Py_ssize_t start, Py_ssize_t len, int optimize);

int PyHP_API_Compile(const char *src, Py_ssize_t len, Py_ssize_t *p_slen, PyHP_API_PageCode_Cell **p_statements, Py_ssize_t *p_blen, char **p_buffer, int optimize);

void PyHP_API_ReleaseCompile(PyHP_API_PageCode_Cell *p_statements, char *p_buffer);


typedef struct
{
    PyObject *name;
    Py_ssize_t buffer_len;
    Py_ssize_t statements_count;
    PyHP_API_PageCode_Cell statements[1];
    const char buffer[1];
} PyHP_API_PageCode;

PyHP_API_PageCode *PyHP_API_AllocPageCode(Py_ssize_t slen, Py_ssize_t blen);

void PyHP_API_FreePageCode(PyHP_API_PageCode *self);

void PyHP_API_ReleasePageCode(PyHP_API_PageCode *self);


#define PyHP_API_PageCode_BASESIZE (sizeof(PyHP_API_PageCode) - sizeof(PyHP_API_PageCode_Cell[1]) - sizeof(const char[1]))
#define PyHP_API_PageCode_EXTRASIZE(SLEN, BLEN) (sizeof(PyHP_API_PageCode_Cell) * (SLEN) + sizeof(const char) * (BLEN))
#define PyHP_API_PageCode_BLEN(PAGECODE) (((PyHP_API_PageCode *)(PAGECODE))->buffer_len)
#define PyHP_API_PageCode_BUFFER(PAGECODE) ((const char *)(((PyHP_API_PageCode *)(PAGECODE))->statements + PyHP_API_PageCode_SLEN(PAGECODE)))
#define PyHP_API_PageCode_SLEN(PAGECODE) (((PyHP_API_PageCode *)(PAGECODE))->statements_count)
#define PyHP_API_PageCode_STATEMENTS(PAGECODE) (((PyHP_API_PageCode *)(PAGECODE))->statements)

#endif /* PyHP_API_COMPILER_H */
