#include <python.h>

#ifndef PY3HP_APP
# define PY3HP_APP

typedef struct {
    PyObject_HEAD
    int is_inited;
    PyObject *pool; /* set */
} AppInstance_Object;

extern AppInstance_Object AppInstance;

#endif /* PY3HP_APP */