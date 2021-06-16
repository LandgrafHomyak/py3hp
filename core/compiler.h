#include <Python.h>
#include "pyhp.h"

#include "parser.h"

#ifndef PyHP_CORE_COMPILER_H
# define PyHP_CORE_COMPILER_H

typedef enum
{
    PyHP_Core_PageCodeType_TEXT = 1,
    PyHP_Core_PageCodeType_EXEC = 2,
    PyHP_Core_PageCodeType_EVAL = 3,
} PyHP_Core_PageCodeType;


typedef struct
{
    PyHP_Core_PageCodeType type;
    union
    {
        PyObject *code;
        struct
        {
            Py_ssize_t start;
            Py_ssize_t len;
        } raw;
    } value;
} PyHP_Core_PageCode_Cell;

PyHP_LOW_API int PyHP_Core_CompileCell(PyHP_Core_PageCode_Cell *dst, int py_version, PyHP_Core_StatementType type, const char *source, Py_ssize_t start, Py_ssize_t len, int optimize);

PyHP_LOW_API int PyHP_Core_Compile(const char *src, Py_ssize_t len, Py_ssize_t *p_slen, PyHP_Core_PageCode_Cell **p_statements, Py_ssize_t *p_blen, char **p_buffer, int optimize);

PyHP_LOW_API void PyHP_Core_ReleaseCompile(PyHP_Core_PageCode_Cell *p_statements, char *p_buffer);


typedef struct
{
    PyObject *name;
    Py_ssize_t buffer_len;
    Py_ssize_t statements_count;
    PyHP_Core_PageCode_Cell statements[1];
    const char buffer[1];
} PyHP_Core_PageCode;

PyHP_LOW_API PyHP_Core_PageCode *PyHP_Core_AllocPageCode(Py_ssize_t slen, Py_ssize_t blen);

PyHP_LOW_API void PyHP_Core_FreePageCode(PyHP_Core_PageCode *self);

PyHP_LOW_API void PyHP_Core_ReleasePageCode(PyHP_Core_PageCode *self);


#define PyHP_Core_PageCode_BASESIZE (sizeof(PyHP_Core_PageCode_Object) - sizeof(PyHP_Core_PageCode_Cell[1]) - sizeof(const char[1]))
#define PyHP_Core_PageCode_EXTRASIZE(SLEN, BLEN) (sizeof(PyHP_Core_PageCode_Cell) * (SLEN) + sizeof(const char) * (BLEN))
#define PyHP_Core_PageCode_BLEN(PAGECODE) (((PyHP_Core_PageCode *)(PAGECODE))->buffer_len)
#define PyHP_Core_PageCode_BUFFER(PAGECODE) ((const char *)(((PyHP_Core_PageCode *)(PAGECODE))->statements + PyHP_Core_PageCode_SLEN(PAGECODE)))
#define PyHP_Core_PageCode_SLEN(PAGECODE) (((PyHP_Core_PageCode *)(PAGECODE))->statements_count)
#define PyHP_Core_PageCode_STATEMENTS(PAGECODE) (((PyHP_Core_PageCode *)(PAGECODE))->statements)

typedef struct
{
    PyObject_VAR_HEAD
    PyHP_Core_PageCode data;
} PyHP_Core_PageCode_Object;

typedef struct
{
    PyObject_HEAD
    PyHP_Core_PageCode_Object *data;
    Py_ssize_t pos;
} PyHP_Core_PageCodeIterator_Object;

PyHP_HIGH_API PyHP_Core_PageCode_Object *PyHP_Core_Compile_Func(PyObject *module, PyObject *args, PyObject *kwargs);

extern PyTypeObject PyHP_Core_PageCode_Type;
extern PyTypeObject PyHP_Core_PageCodeIterator_Type;
#endif /* PyHP_CORE_COMPILER_H */
