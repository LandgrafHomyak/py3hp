#include <Python.h>
#include "compiler.h"

#ifndef PyHP_EXECUTOR_H
#define PyHP_EXECUTOR_H

#ifdef __cplusplus
extern "C" {
#endif

int PyHP_ExecEmbed(PyHP_CompilerStateWithParent *code_iterator, PyObject *out_file, PyObject *globals);
int PyHP_CompilerOrString_Converter(PyObject *src, PyHP_CompilerState_Object **dst);

#ifdef __cplusplus
}
#endif
#endif