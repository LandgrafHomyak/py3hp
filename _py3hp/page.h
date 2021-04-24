#include <Python.h>

#ifndef PY3HP_PAGE_H
#define PY3HP_PAGE_H

typedef struct
{
    PyObject_HEAD
    PyObject *source; /* str */
    PyObject *url; /* str or NULL */
    PyThreadState *sub_interpreter;
    PyObject *finish_callback; /* callable or NULL*/
    double timeout;
    struct
    {
        int running: 1;
        int finished: 1;
        int lost_reference: 1;
    } flags;
} Page_Object;

extern PyTypeObject Page_Type;

#endif /* PY3HP_PAGE_H */
