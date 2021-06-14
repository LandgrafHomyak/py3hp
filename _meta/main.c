#include <Python.h>
#include <py3hp.h>


static PyModuleDef module_def = {
        PyModuleDef_HEAD_INIT,
        .m_name="py3hp._meta",
        .m_doc = "",
        .m_size = -1,
};

PyMODINIT_FUNC PyInit__meta(void)
{
    PyObject *module;
    PyObject *object;
    char *rls;

    module = PyModule_Create(&module_def);
    if (module == NULL)
    {
        return NULL;
    }

    object = PyUnicode_FromFormat("%s %s", Py3hp_VERSION, Py_GetCompiler());
    if (object == NULL)
    {
        return NULL;
    }

    if (PyModule_AddObject(module, "version", object) != 0)
    {
        return NULL;
    }

    object = PyLong_FromLong(Py3hp_VERSION_HEX);
    if (object == NULL)
    {
        return NULL;
    }

    if (PyModule_AddObject(module, "hexversion", object) != 0)
    {
        return NULL;
    }

#if Py3hp_RELEASE_LEVEL == PY_RELEASE_LEVEL_ALPHA
    rls = "alpha";
#elif Py3hp_RELEASE_LEVEL == PY_RELEASE_LEVEL_BETA
    rls = "beta";
#elif Py3hp_RELEASE_LEVEL == PY_RELEASE_LEVEL_GAMMA
    rls = "candidate";
#elif Py3hp_RELEASE_LEVEL == PY_RELEASE_LEVEL_FINAL
    rls = "final";
#endif

    object = Py_BuildValue("iiisi", Py3hp_MAJOR_VERSION, Py3hp_MINOR_VERSION, Py3hp_MICRO_VERSION, rls, Py3hp_RELEASE_SERIAL);
    if (object == NULL)
    {
        return NULL;
    }

    if (PyModule_AddObject(module, "version_info", object) != 0)
    {
        return NULL;
    }

    return module;

}