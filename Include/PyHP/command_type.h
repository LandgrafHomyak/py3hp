#include <Python.h>

#ifndef PyHP_Command_TYPE_H
# define PyHP_Command_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PyHP_CommandType
{
    PyHP_CommandType_NONE = 0,
    PyHP_CommandType_TEXT = 1,
    PyHP_CommandType_EVAL = 2,
    PyHP_CommandType_EXEC = 3
} PyHP_CommandType;

typedef struct PyHP_CommandType_Object
{
    PyObject_HEAD
    char *name;
    enum PyHP_CommandType value;
} PyHP_CommandType_Object;

extern PyTypeObject PyHP_CommandType_Type;
extern const PyHP_CommandType_Object *const PyHP_CommandTypeObject_NONE;
extern const PyHP_CommandType_Object *const PyHP_CommandTypeObject_TEXT;
extern const PyHP_CommandType_Object *const PyHP_CommandTypeObject_EVAL;
extern const PyHP_CommandType_Object *const PyHP_CommandTypeObject_EXEC;

const PyHP_CommandType_Object *PyHP_CommandType_ToObject(PyHP_CommandType);

#ifdef __cplusplus
}
#endif

#endif /* PyHP_Command_TYPE_H */
