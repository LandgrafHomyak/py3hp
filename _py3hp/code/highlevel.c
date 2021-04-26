#include <Python.h>

#include "highlevel.h"
#include "class.h"
#include "parser.h"

PyObject *Py3hp_Compile_O(PyObject *module, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"", "filename", "optimize", NULL};

    PyObject *source;
    PyObject *flie_like;
    PyObject *bytes;
    PyObject *filename = NULL;
    int optimize = -1;
    PyTypeObject *cls;
    char *string;
    Py_ssize_t start;
    Py_ssize_t len;
    Py_ssize_t i;
    code_block_slice ref;
    char *program_string;
    char *buffer;
    Py_ssize_t new_len;
    Py3hpCode_Command *commands;
    Py_ssize_t commands_len;
    Py_ssize_t commands_max_len;
    Py_ssize_t alligned_length;
    Py3hpCode_Object *self;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|U$i", kw_list, &source, &filename, &optimize))
    {
        return NULL;
    }

    if (PyUnicode_Check(source))
    {
        Py_INCREF(source);
        cls = &Py3hpCodeStr_Type;
    }
    else if (PyBytes_Check(source))
    {
        Py_INCREF(source);
        cls = &Py3hpCodeBytes_Type;
    }
    else
    {
        flie_like = source;
        source = PyObject_CallMethod(flie_like, "read", "");
        if (source == NULL)
        {
            return NULL;
        }

        if (PyUnicode_Check(source))
        {
            cls = &Py3hpCodeStr_Type;
        }
        else if (PyUnicode_Check(source))
        {
            cls = &Py3hpCodeBytes_Type;
        }
        else
        {
            Py_DECREF(source);
            PyErr_Format(
                    PyExc_TypeError,
                    "method read of file-like object must return str or bytes"
            );
            return NULL;
        }
    }

    if (PyUnicode_Check(source))
    {
        bytes = PyUnicode_AsEncodedString(source, "utf-8", NULL);
        Py_DECREF(source);
        if (bytes == NULL)
        {
            return NULL;
        }
    }
    else
    {
        bytes = source;
    }

    string = PyBytes_AsString(bytes);

    len = PyBytes_Size(bytes);

    buffer = PyMem_Malloc(2 * (len + 1));
    if (buffer == NULL)
    {
        Py_DECREF(bytes);
        PyErr_NoMemory();
        return NULL;
    }
    program_string = buffer + len + 1;

    commands_max_len = 1;
    commands_len = 0;
    commands = PyMem_Malloc(sizeof(Py3hpCode_Command));
    if (buffer == NULL)
    {
        PyMem_Free(bytes);
        Py_DECREF(bytes);
        PyErr_NoMemory();
        return NULL;
    }

    new_len = 0;
    start = 0;

#define add(TYPE, LEN, POINTER) \
    if (commands_len >= commands_max_len)\
        {commands_max_len <<= 1;commands = PyMem_Realloc(commands, commands_max_len * sizeof(Py3hpCode_Command));}\
    if (commands != NULL) \
        {commands[commands_len].type = (TYPE), commands[commands_len].len = (LEN),  commands[commands_len].pointer.any = (void *)(POINTER), commands_len++;}\
    else

    while (start < len)
    {


        ref = Code_Parser_NextBlock(string, start, len);


        if (ref.type == Py3hpCode_Command_NOT_FOUND)
        {
            ref.raw_end = len;
        }
        add(Py3hpCode_Command_RAW, ref.raw_end - start, NULL)
        {
            PyMem_Free(buffer);
            Py_DECREF(bytes);
            PyErr_NoMemory();
            return NULL;
        }
        memcpy(buffer + new_len, string + start, ref.raw_end - start);
        new_len += ref.raw_end - start;
        if (ref.type == Py3hpCode_Command_NOT_FOUND)
        {
            break;
        }

        add(ref.type, 0, NULL)
        {
            PyMem_Free(buffer);
            Py_DECREF(bytes);
            PyErr_NoMemory();
            return NULL;
        }
        alligned_length = Code_Parser_AlignIndent(string, program_string, ref.code_start, ref.code_end, 8);
        if (alligned_length == -1)
        {
            PyMem_Free(commands);
            PyMem_Free(buffer);
            Py_DECREF(bytes);
            return NULL;
        }
        program_string[alligned_length] = '\0';

        if (ref.type == Py3hpCode_Command_EXEC)
        {
            commands[commands_len - 1].pointer.code = Py_CompileString(program_string, "<py3hp inline frame>", Py_file_input);
        }
        else if (ref.type == Py3hpCode_Command_EVAL)
        {
//            memcpy(program_string, string + ref.code_start, ref.code_end - ref.code_start);
//            program_string[ref.code_end - ref.code_start] = '\0';

            commands[commands_len - 1].pointer.code = Py_CompileString(program_string, "<py3hp block frame>", Py_eval_input);
        }


        if (commands[commands_len - 1].pointer.code == NULL)
        {
            PyMem_Free(commands);
            PyMem_Free(buffer);
            Py_DECREF(bytes);
            return NULL;
        }

        start = ref.raw_start;
    }
#undef end
    Py_DECREF(bytes);
    if ((buffer = PyMem_Realloc(buffer, new_len)) == NULL)
    {
        PyMem_Free(commands);
        PyErr_NoMemory();
        return NULL;
    }

    self = (Py3hpCode_Object *) (cls->tp_alloc(cls, commands_len));
    if (self == NULL)
    {
        PyMem_Free(commands);
        PyMem_Free(buffer);
        PyErr_NoMemory();
        return NULL;
    }

    self->file_name = filename;
    self->buffer = buffer;
    memcpy(&(self->commands), commands, commands_len * sizeof(Py3hpCode_Command));
    PyMem_Free(commands);

    start = 0;
    for (i = 0; i < commands_len; i++)
    {
        if (self->commands[i].type == Py3hpCode_Command_RAW)
        {
            self->commands[i].pointer.raw = buffer + start;
            start += self->commands[i].len;
        }
    }

    return (PyObject *) self;
}