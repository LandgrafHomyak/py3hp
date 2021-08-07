#include "iterator_meta.h"
#include "parser.h"

#ifndef PyHP_PREPARE_H
#define PyHP_PREPARE_H
#ifdef __cplusplus
extern "C" {
#endif
typedef struct PyHP_PrepareState
{
    PyHP_IteratorHead head;
    PyHP_ParserState *parser_state;
    void *prepared_string;
} PyHP_PrepareState;

typedef struct PyHP_PrepareStateWithParent
{
    PyHP_PrepareState self;
    PyHP_ParserState parent;
} PyHP_PrepareStateWithParent;

typedef struct PyHP_PrepareState_Object
{
    PyObject_HEAD
    PyObject *parent;
    PyHP_PrepareStateWithParent data;
} PyHP_PrepareState_Object;

extern PyHP_IteratorMeta_Object PyHP_PrepareIterator_Type;

int PyHP_Prepare_BindParser(PyHP_PrepareState *self, PyHP_ParserState *parent);

int PyHP_Prepare_WrapParser(PyHP_PrepareStateWithParent *self, PyHP_ParserState *parent);

int PyHP_Prepare_FromParser(PyHP_PrepareStateWithParent *self, PyHP_ParserState *parent);

int PyHP_Prepare_Copy(PyHP_PrepareStateWithParent *self, PyHP_PrepareStateWithParent *dst);

void PyHP_Prepare_Free(PyHP_PrepareStateWithParent *self);

int PyHP_Prepare_Next(PyHP_PrepareStateWithParent *self, PyHP_PrepareResult *dst);

#define PyHP_PrepareIterator_DATA(OBJECT) (((PyHP_PrepareIterator_Object *)(OBJECT))->data)

#ifdef __cplusplus
}
#endif
#endif /* PyHP_PREPARE_H */
