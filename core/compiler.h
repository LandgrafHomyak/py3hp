#include <Python.h>
#include "py3hp.h"

#include "parser.h"

#ifndef PY3HP_CORE_COMPILER_H
# define PY3HP_CORE_COMPILER_H

typedef enum
{
    Py3hp_Core_PageCodeType_TEXT = 1,
    Py3hp_Core_PageCodeType_EXEC = 2,
    Py3hp_Core_PageCodeType_EVAL = 3,
} Py3hp_Core_PageCodeType;


typedef struct
{
    Py3hp_Core_PageCodeType type;
    union
    {
        PyObject *code;
        struct
        {
            Py_ssize_t start;
            Py_ssize_t len;
        } raw;
    } value;
} Py3hp_Core_PageCode_Cell;

PY3HP_LOW_API int Py3hp_Core_CompileCell(Py3hp_Core_PageCode_Cell *dst, int py_version, Py3hp_Core_StatementType type, const char *source, Py_ssize_t start, Py_ssize_t len, int optimize);

PY3HP_LOW_API int Py3hp_Core_Compile(const char *src, Py_ssize_t len, Py_ssize_t *p_slen, Py3hp_Core_PageCode_Cell **p_statements, Py_ssize_t *p_blen, char **p_buffer, int optimize);

PY3HP_LOW_API void Py3hp_Core_ReleaseCompile(Py3hp_Core_PageCode_Cell *p_statements, char *p_buffer);


typedef struct
{
    PyObject_VAR_HEAD
    PyObject *name;
    Py_ssize_t buffer_len;
    Py_ssize_t statements_count;
    Py3hp_Core_PageCode_Cell statements[1];
    const char buffer[1];
} Py3hp_Core_PageCode;

PY3HP_LOW_API Py3hp_Core_PageCode *Py3hp_Core_AllocPageCode(Py_ssize_t slen, Py_ssize_t blen);

PY3HP_LOW_API void Py3hp_Core_FreePageCode(Py3hp_Core_PageCode *self);

PY3HP_LOW_API void Py3hp_Core_ReleasePageCode(Py3hp_Core_PageCode *self);


#define Py3hp_Core_PageCode_BASESIZE (sizeof(Py3hp_Core_PageCode_Object) - sizeof(Py3hp_Core_PageCode_Cell[1]) - sizeof(const char[1]))
#define Py3hp_Core_PageCode_EXTRASIZE(SLEN, BLEN) (sizeof(Py3hp_Core_PageCode_Cell) * (SLEN) + sizeof(const char) * (BLEN))
#define Py3hp_Core_PageCode_BLEN(PAGECODE) (((Py3hp_Core_PageCode *)(PAGECODE))->buffer_len)
#define Py3hp_Core_PageCode_BUFFER(PAGECODE) ((const char *)(((Py3hp_Core_PageCode *)(PAGECODE))->statements + Py3hp_Core_PageCode_SLEN(PAGECODE)))
#define Py3hp_Core_PageCode_SLEN(PAGECODE) (((Py3hp_Core_PageCode *)(PAGECODE))->statements_count)
#define Py3hp_Core_PageCode_STATEMENTS(PAGECODE) (((Py3hp_Core_PageCode *)(PAGECODE))->statements)

typedef struct
{
    PyObject_VAR_HEAD
    Py3hp_Core_PageCode data;
} Py3hp_Core_PageCode_Object;

typedef struct
{
    PyObject_HEAD
    Py3hp_Core_PageCode_Object *data;
    Py_ssize_t pos;
} Py3hp_Core_PageCodeIterator_Object;

PY3HP_HIGH_API Py3hp_Core_PageCode_Object *Py3hp_Core_Compile_Func(PyObject *module, PyObject *args, PyObject *kwargs);

extern PyTypeObject Py3hp_Core_PageCode_Type;
extern PyTypeObject Py3hp_Core_PageCodeIterator_Type;
#endif /* PY3HP_CORE_COMPILER_H */
