#include <Python.h>

#include <PyHP.h>

static void PyHP_IteratorMeta_Dealloc(PyHP_IteratorMeta_Object *cls)
{
    if (cls->tp.tp_flags & Py_TPFLAGS_HEAPTYPE)
    {
        PyType_Type.tp_dealloc((PyObject *) &(cls->tp));
    }
}

PyTypeObject PyHP_IteratorMeta_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "pyhp.types.iterator_meta",
        .tp_basicsize = sizeof(PyHP_IteratorMeta_Object),
        .tp_dealloc = (destructor) PyHP_IteratorMeta_Dealloc
};