#include <Python.h>
#include "statement_type.h"

#ifndef PyHP_PREPARE_RESULT_H
#define PyHP_PREPARE_RESULT_H
#ifdef __cplusplus
extern "C" {
#endif
typedef struct PyHP_PrepareResult
{
    PyHP_StatementType type;
    void *string;
} PyHP_PrepareResult;

typedef struct PyHP_PrepareResult_Object
{
    PyObject_HEAD
    PyHP_PrepareResult data;
    PyObject *raw_string;
} PyHP_PrepareResult_Object;

extern PyTypeObject PyHP_PrepareResult_Type;

int PyHP_PrepareResult_Converter(PyHP_PrepareResult_Object *src, PyHP_PrepareResult *dst);
#ifdef __cplusplus
}
#endif
#endif /* PyHP_PREPARE_RESULT_H */
