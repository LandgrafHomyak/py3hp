#include <Python.h>

#ifndef PyHP_MODULES_H
#define PyHP_MODULES_H

PyMODINIT_FUNC PyHPInit_parser(void);
PyMODINIT_FUNC PyHPInit_compiler(void);
PyMODINIT_FUNC PyHPInit_types(void);

#endif /* PyHP_MODULES_H */
