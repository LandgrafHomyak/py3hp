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

PY3HP_LOW_API int Py3hp_Core_Compile(Py3hp_Core_PageCode_Cell *dst, int py_version, Py3hp_Core_StatementType type, const char *source, Py_ssize_t start, Py_ssize_t len, int optimize);

typedef struct
{
    PyObject_VAR_HEAD
    PyObject *name;
    Py_ssize_t buffer_len;
    Py_ssize_t statements_count;
    Py3hp_Core_PageCode_Cell statements[1];
    const char buffer[1];
} Py3hp_Core_PageCode_Object;

#define Py3hp_Core_PageCode_BLEN(OBJECT) (((Py3hp_Core_PageCode_Object *)(OBJECT))->buffer_len)
#define Py3hp_Core_PageCode_BUFFER(OBJECT) ((const char *)(((Py3hp_Core_PageCode_Object *)(OBJECT))->statements + Py3hp_Core_PageCode_SLEN(OBJECT)))
#define Py3hp_Core_PageCode_SLEN(OBJECT) (((Py3hp_Core_PageCode_Object *)(OBJECT))->statements_count)
#define Py3hp_Core_PageCode_STATEMENTS(OBJECT) (((Py3hp_Core_PageCode_Object *)(OBJECT))->statements)

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
