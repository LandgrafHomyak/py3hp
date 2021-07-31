#ifndef PyHP_ITERATOR_META_H
#define PyHP_ITERATOR_META_H

typedef void (*PyHP_Iterator_Next_FuncType)(void *, void *);

typedef void (*PyHP_Iterator_Free_FuncType)(void *);

typedef int (*PyHP_Iterator_Copy_FuncType)(void *, void *);

typedef struct PyHP_IteratorMeta
{
    PyHP_Iterator_Next_FuncType im_next;
    PyHP_Iterator_Free_FuncType im_free;
    PyHP_Iterator_Copy_FuncType im_copy;
} PyHP_IteratorMeta;

typedef struct PyHP_IteratorMeta_Object
{
    PyTypeObject tp;
    PyHP_IteratorMeta im;
} PyHP_IteratorMeta_Object;

PyTypeObject PyHP_IteratorMeta_Type;

#endif /* PyHP_ITERATOR_META_H */
