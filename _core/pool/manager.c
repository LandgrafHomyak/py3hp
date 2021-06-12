#include <Python.h>

#include "manager.h"
#include "class.h"


pools_list_type pools_list = {0, 0};


Py3hpPool_Object *Pool_Manager_GetDefaultPool(void)
{
    PyThreadState *current;
    Py_ssize_t i;
    Py_ssize_t j;
    Py3hpPool_Object *pool = NULL;
/*
    int _release_gil;
    PyGILState_STATE _gil_state;

    if (!PyGILState_Check())
    {
        _release_gil = 1;
        _gil_state = PyGILState_Ensure();
    }
    else
    {
        _release_gil = 0;
    }
*/
    current = PyThreadState_Get();


    for (i = 0; i < pools_list.len; i++)
    {
        if (pools_list.list[i]->master->ts == current)
        {
            pool = pools_list.list[i];
            goto ret;
        }

        for (j = 0; j < pools_list.list[i]->len; j++)
        {
            if (pools_list.list[i]->array[j]->ts == current)
            {
                pool = pools_list.list[i];
                goto ret;
            }
        }
    }
    ret:
/*
    if (_release_gil)
    {
        PyGILState_Release(_gil_state);
    }
*/
    return pool;
}

Py3hpPool_Object *Pool_Manager_GetDefaultPoolOrCreate(void)
{
    Py3hpPool_Object *defpool;
    PyThreadState *current;
/*
    int _release_gil;
    PyGILState_STATE _gil_state;


    if (!PyGILState_Check())
    {
        _release_gil = 1;
        _gil_state = PyGILState_Ensure();
    }
    else
    {
        _release_gil = 0;
    }
*/

    defpool = Pool_Manager_GetDefaultPool();

    if (defpool != NULL)
    {
        goto ret;
    }

    current = PyThreadState_Get();

    defpool = Py3hpPool_FromMaster(current);
    if (defpool == NULL)
    {

    }
    Py3hpPool_MakeDefault(defpool);
/*
    if (_release_gil)
    {
        PyGILState_Release(_gil_state);
    }
*/
    ret:
    return defpool;
}
