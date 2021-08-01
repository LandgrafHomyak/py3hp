#include <Python.h>

#ifndef PyHP_MODULES_H
#define PyHP_MODULES_H

#ifdef __cplusplus
extern "C" {
#endif

PyMODINIT_FUNC PyHPInit_parser(void);

PyMODINIT_FUNC PyHPInit_compiler(void);

PyMODINIT_FUNC PyHPInit_types(void);

#ifdef __cplusplus
}
#endif

#endif /* PyHP_MODULES_H */
