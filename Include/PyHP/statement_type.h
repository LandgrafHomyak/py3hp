#include <Python.h>

#ifndef PyHP_STATEMENT_TYPE_H
# define PyHP_STATEMENT_TYPE_H

typedef enum PyHP_StatementType
{
    PyHP_StatementType_NONE = 0,
    PyHP_StatementType_RAW = 1,
    PyHP_StatementType_INLINE = 2,
    PyHP_StatementType_BLOCK = 3
} PyHP_StatementType;

typedef struct PyHP_StatementType_Object
{
    PyObject_HEAD
    char *name;
    enum PyHP_StatementType value;
} PyHP_StatementType_Object;

extern PyTypeObject PyHP_StatementType_Type;
extern const PyHP_StatementType_Object *const PyHP_StatementTypeObject_NONE;
extern const PyHP_StatementType_Object *const PyHP_StatementTypeObject_RAW;
extern const PyHP_StatementType_Object *const PyHP_StatementTypeObject_INLINE;
extern const PyHP_StatementType_Object *const PyHP_StatementTypeObject_BLOCK;

const PyHP_StatementType_Object *PyHP_StatementType_ToObject(PyHP_StatementType);

#endif /* PyHP_STATEMENT_TYPE_H */
