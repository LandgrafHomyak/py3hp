#include <Python.h>

#include "class.h"

#ifndef PY3HP_POOL_MANAGER_H
#define PY3HP_POOL_MANAGER_H


typedef struct
{
    Py_ssize_t len;
    Py3hpPool_Object **list;
} pools_list_type;

extern pools_list_type pools_list;

Py3hpPool_Object *Pool_Manager_GetDefaultPool(void);

Py3hpPool_Object *Pool_Manager_GetDefaultPoolOrCreate(void);

#endif /* PY3HP_POOL_MANAGER_H */
