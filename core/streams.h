#include <Python.h>
#include "pyhp.h"

#ifndef PyHP_CORE_STREAMS_H
#define PyHP_CORE_STREAMS_H

typedef struct
{
    char *buffer;
    Py_ssize_t allocated;
    Py_ssize_t endpos;
    Py_ssize_t pos;
} PyHP_Core_Stream;

PyHP_LOW_API int PyHP_Core_Stream_Extend(PyHP_Core_Stream *stream, Py_ssize_t new_size);

PyHP_LOW_API int PyHP_Core_Stream_Write(PyHP_Core_Stream *stream, const char *string, Py_ssize_t start, Py_ssize_t len);

PyHP_LOW_API int PyHP_Core_Stream_Seek(PyHP_Core_Stream *stream, Py_ssize_t pos, int whence);

PyHP_LOW_API const char *PyHP_Core_Stream_Read(PyHP_Core_Stream *stream, Py_ssize_t *len);

PyHP_LOW_API const char *PyHP_Core_Stream_ReadN(PyHP_Core_Stream *stream, Py_ssize_t *len);

PyHP_LOW_API int PyHP_Core_Stream_Open(PyHP_Core_Stream *stream);

PyHP_LOW_API int PyHP_Core_Stream_Close(PyHP_Core_Stream *stream);

typedef struct
{
    PyObject_HEAD
    PyObject *string;
    Py_ssize_t pos;
} PyHP_Core_StdinPreset_Object;
typedef struct
{
    PyObject_HEAD
    PyHP_Core_Stream data;
} PyHP_Core_Stdout_Object;

typedef struct
{
    PyObject_HEAD
    PyHP_Core_Stream data;
} PyHP_Core_Stderr_Object;

typedef struct
{
    PyObject_HEAD
    PyHP_Core_Stdout_Object *data;
} PyHP_Core_StdoutPipe_Object;

typedef struct
{
    PyObject_HEAD
    PyHP_Core_Stderr_Object *data;
} PyHP_Core_StderrPipe_Object;

extern PyTypeObject PyHP_Core_BaseStream_Type;
extern PyTypeObject PyHP_Core_StdinPreset_Type;
extern PyTypeObject PyHP_Core_Stdout_Type;
extern PyTypeObject PyHP_Core_Stderr_Type;
extern PyTypeObject PyHP_Core_StdoutPipe_Type;
extern PyTypeObject PyHP_Core_StderrPipe_Type;

#endif /* PyHP_CORE_STREAMS_H */
