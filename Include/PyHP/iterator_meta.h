#ifndef PyHP_ITERATOR_META_H
#define PyHP_ITERATOR_META_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyHP_IteratorHead PyHP_IteratorHead;

typedef int (*PyHP_Iterator_Init_FuncType)(PyHP_IteratorHead *, void *);

typedef int (*PyHP_Iterator_Next_FuncType)(PyHP_IteratorHead *, void *);

typedef void (*PyHP_Iterator_Free_FuncType)(PyHP_IteratorHead *);

typedef int (*PyHP_Iterator_Copy_FuncType)(PyHP_IteratorHead *, PyHP_IteratorHead *);

typedef struct PyHP_IteratorMeta
{
    /*
    PyHP_Iterator_Init_FuncType im_init;
    */
    PyHP_Iterator_Next_FuncType im_next;
    PyHP_Iterator_Free_FuncType im_free;
    PyHP_Iterator_Copy_FuncType im_copy;
} PyHP_IteratorMeta;

typedef struct PyHP_IteratorMeta_Object
{
    PyTypeObject tp;
    PyHP_IteratorMeta im;
} PyHP_IteratorMeta_Object;

extern PyTypeObject PyHP_IteratorMeta_Type;

struct PyHP_IteratorHead
{
    PyHP_IteratorMeta *meta_info;
};

/*
#define PyHP_Iterator_Init(DST, DATA) (((PyHP_IteratorHead *)(SELF))->meta_info->im_init((PyHP_IteratorHead *)(DST), (void *)(DATA)))
*/
#define PyHP_Iterator_Next(SELF, DST) (((PyHP_IteratorHead *)(SELF))->meta_info->im_next((PyHP_IteratorHead *)(SELF), (void *)(DST)))
#define PyHP_Iterator_Free(SELF) (((PyHP_IteratorHead *)(SELF))->meta_info->im_free((PyHP_IteratorHead *)(SELF)))
#define PyHP_Iterator_Copy(SELF, DST) (((PyHP_IteratorHead *)(SELF))->meta_info->im_copy((PyHP_IteratorHead *)(SELF), (PyHP_IteratorHead *)(DST)))

#ifdef __cplusplus
}
#endif

#endif /* PyHP_ITERATOR_META_H */
