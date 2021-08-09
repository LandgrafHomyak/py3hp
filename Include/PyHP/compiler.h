#include <Python.h>
#include "prepare.h"

#ifndef PyHP_COMPILER_H
#define PyHP_COMPILER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyHP_CompilerState
{
    PyHP_IteratorHead head;
    PyHP_PrepareStateWithParent *prepare_state;
} PyHP_CompilerState;

typedef struct PyHP_CompilerStateWithParent
{
    PyHP_CompilerState self;
    PyHP_PrepareStateWithParent parent;
} PyHP_CompilerStateWithParent;

typedef struct PyHP_CompilerState_Object
{
    PyObject_HEAD
    PyObject *parent;
    PyHP_CompilerStateWithParent data;
} PyHP_CompilerState_Object;

int PyHP_Compiler_BindPrepare(PyHP_CompilerState *self, PyHP_PrepareStateWithParent *parent);

int PyHP_Compiler_WrapPrepare(PyHP_CompilerStateWithParent *self, PyHP_PrepareStateWithParent *parent);

int PyHP_Compiler_FromPrepare(PyHP_CompilerStateWithParent *self, PyHP_PrepareStateWithParent *parent);

int PyHP_Compiler_Next(PyHP_CompilerStateWithParent *self, PyHP_Command *dst);

int PyHP_Compiler_Copy(PyHP_CompilerStateWithParent *self, PyHP_CompilerStateWithParent *dst);

void PyHP_Compiler_Free(PyHP_CompilerStateWithParent *self);

extern PyHP_IteratorMeta_Object PyHP_CompilerIterator_Type;

#define PyHP_CompilerIterator_DATA(OBJECT) (((PyHP_CompilerState_Object *)(OBJECT))->data)


#ifdef __cplusplus
}
#endif

#endif /* PyHP_COMPILER_H */
