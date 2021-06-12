#ifndef PY3HP_CORE_H
# define PY3HP_CORE_H

# define PY3HP_LOW_API
# define PY3HP_HIGH_API

#if PY_VERSION_HEX >= 0x02030000
#else
#define PyMODINIT_FUNC DL_EXPORT(void)
#endif
#if PY_VERSION_HEX >= 0x02050000
#else
#define Py_ssize_t int
#define T_PYSSIZET T_INT
#endif
#if PY_VERSION_HEX >= 0x03000000
#else
#define Py_TYPE(OBJECT) (((PyObject *)(OBJECT))->ob_type)
#define Py_SIZE(OBJECT) (((PyVarObject *)(OBJECT))->ob_size)
#endif
#endif /* PY3HP_CORE_H */
