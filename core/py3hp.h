#ifndef PY3HP_CORE_H
# define PY3HP_CORE_H

# define PY3HP_LOW_API
# define PY3HP_HIGH_API

#if PY_VERSION_HEX >= 0x02020000
#else
#define PyMODINIT_FUNC DL_EXPORT(void)
#endif
#if PY_VERSION_HEX >= 0x02050000
#else
#define Py_ssize_t int
#endif
#endif /* PY3HP_CORE_H */
