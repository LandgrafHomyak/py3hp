#include <Python.h>
#include "py3hp.h"

#ifndef PY3HP_CORE_STREAMS_H
#define PY3HP_CORE_STREAMS_H

typedef struct
{
    char *buffer;
    Py_ssize_t allocated;
    Py_ssize_t endpos;
    Py_ssize_t pos;
} Py3hp_Core_Stream;

PY3HP_LOW_API int Py3hp_Core_Stream_Extend(Py3hp_Core_Stream *stream, Py_ssize_t new_size);

PY3HP_LOW_API int Py3hp_Core_Stream_Write(Py3hp_Core_Stream *stream, const char *string, Py_ssize_t start, Py_ssize_t len);

PY3HP_LOW_API int Py3hp_Core_Stream_Seek(Py3hp_Core_Stream *stream, Py_ssize_t pos, int whence);

PY3HP_LOW_API const char *Py3hp_Core_Stream_Read(Py3hp_Core_Stream *stream, Py_ssize_t *len);

PY3HP_LOW_API const char *Py3hp_Core_Stream_ReadN(Py3hp_Core_Stream *stream, Py_ssize_t *len);

PY3HP_LOW_API int Py3hp_Core_Stream_Open(Py3hp_Core_Stream *stream);

PY3HP_LOW_API int Py3hp_Core_Stream_Close(Py3hp_Core_Stream *stream);

typedef struct
{
    PyObject_HEAD
    PyObject *string;
    Py_ssize_t pos;
} Py3hp_Core_StdinPreset_Object;
typedef struct
{
    PyObject_HEAD
    Py3hp_Core_Stream data;
} Py3hp_Core_Stdout_Object;

typedef struct
{
    PyObject_HEAD
    Py3hp_Core_Stream data;
} Py3hp_Core_Stderr_Object;

typedef struct
{
    PyObject_HEAD
    Py3hp_Core_Stdout_Object *data;
} Py3hp_Core_StdoutPipe_Object;

typedef struct
{
    PyObject_HEAD
    Py3hp_Core_Stderr_Object *data;
} Py3hp_Core_StderrPipe_Object;

extern PyTypeObject Py3hp_Core_BaseStream_Type;
extern PyTypeObject Py3hp_Core_StdinPreset_Type;
extern PyTypeObject Py3hp_Core_Stdout_Type;
extern PyTypeObject Py3hp_Core_Stderr_Type;
extern PyTypeObject Py3hp_Core_StdoutPipe_Type;
extern PyTypeObject Py3hp_Core_StderrPipe_Type;

#endif /* PY3HP_CORE_STREAMS_H */
