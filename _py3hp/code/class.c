#include <Python.h>
#include "class.h"
#include "parser.h"

static char anonymous_file[] = "<anonymous>";

static PyObject *anonymous_file_o = NULL;

static PyObject *compile_function = NULL;


int Code_Class_Init(void)
{
    PyObject *module;
    if (PyType_Ready(&Py3hpCode_Type))
    {
        return -1;
    }

    if ((anonymous_file_o = PyUnicode_FromString(anonymous_file)) == NULL)
    {
        return -1;
    }

    if ((module = PyImport_AddModule("builtins")) == NULL)
    {
        return -1;
    }
    if ((compile_function = PyObject_GetAttrString(module, "compile")) == NULL)
    {
        return -1;
    }
    module = NULL;
    return 0;
}

void Code_Class_Free(void)
{
    Py_XDECREF(anonymous_file_o);
    Py_XDECREF(compile_function);
}

static void Py3hpCode_Dealloc(Py3hpCode_Object *self)
{
    Py_ssize_t i;
    Py_ssize_t len;

    for (i = 0, len = Py_SIZE(self); i < len; i++)
    {
        Py_DECREF(self->commands[i]);
    }
    Py_TYPE(self)->tp_free(self);
}

static PyObject *Code_Class_Compile(PyObject *str, int type, Py_ssize_t frame_no, int optimization_level)
{
    PyObject *frame_name;
    char *mode;
    PyObject *co;

    switch (type)
    {
        case BLOCK_CODE:
            mode = "exec";
            break;
        case INLINE_CODE:
            mode = "eval";
            break;
        default:
            PyErr_Format(
                    PyExc_TypeError,
                    "unexpected type of py3hp frame '%d'",
                    type
            );
            return NULL;
    }


    frame_name = PyUnicode_FromFormat("<py3hp frame #%zd>", frame_no);
    if (frame_name == NULL)
    {
        return NULL;
    }
    co = PyObject_CallFunction(compile_function, "OOsiii", str, frame_name, mode, 0, 0, optimization_level);
    Py_DECREF(frame_name);
    if (co == NULL)
    {
        return NULL;
    }

    return co;
}

PyObject *Py3hpCode_CompileS(PyObject *_cls_or_module, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"", "file", "optimize", NULL};
    PyObject *source;
    Py_ssize_t source_len;
    PyObject *file_name = anonymous_file_o;
    Py_ssize_t start;
    PyObject *commands;
    PyObject *_slice;
    PyObject *slice;
    PyObject *co;
    code_block_slice ref;
    Py_ssize_t frame_no;
    int optimization_level = -1;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "U|$Ui", kw_list, &source, &file_name, &optimization_level))
    {
        return NULL;
    }

    commands = PyList_New(0);
    if (commands == NULL)
    {
        return NULL;
    }


#if PY_VERSION_HEX >= 0x03030000
    source_len = PyUnicode_GET_LENGTH(source);
#else
    source_len = PyUnicode_GET_SIZE(source);
#endif
    start = 0;
    frame_no = 1;
    while (start < source_len)
    {
        ref = Code_Parser_NextBlock(source, start);
        if (ref.type == ERROR)
        {
            Py_DECREF(commands);
            return NULL;
        }

        if (ref.type == NOT_FOUND)
        {
            ref.raw_end = source_len;
        }

#if PY_VERSION_HEX >= 0x03030000
        slice = PyUnicode_Substring(source, start, ref.raw_end);
        if (slice == NULL)
        {
            Py_DECREF(commands);
            return NULL;
        }
#else
        _slice = PyObject_CallFunction((PyObject *) &PySlice_Type, "nn", start, ref.raw_end);
        if (_slice == NULL)
        {
            Py_DECREF(commands);
            return NULL;
        }

        slice = PyObject_GetItem(source, _slice);
        Py_DECREF(_slice);
        if (source == NULL)
        {
            Py_DECREF(commands);
            return NULL;
        }
#endif
        PyList_Append(commands, slice);
        Py_DECREF(slice);

        if (ref.type == NOT_FOUND)
        {
            start = source_len;
            break;
        }

#if PY_VERSION_HEX >= 0x03030000
        slice = PyUnicode_Substring(source, ref.code_start, ref.code_end);
        if (slice == NULL)
        {
            Py_DECREF(commands);
            return NULL;
        }
#else
        _slice = PyObject_CallFunction((PyObject *) &PySlice_Type, "nn", ref.code_start, ref.code_end);
        if (_slice == NULL)
        {
            Py_DECREF(commands);
            return NULL;
        }

        slice = PyObject_GetItem(source, _slice);
        Py_DECREF(_slice);
        if (source == NULL)
        {
            Py_DECREF(commands);
            return NULL;
        }
#endif
        if (ref.type == BLOCK_CODE)
        {
            co = Code_Parser_AlignIndent(slice);
            Py_DECREF(slice);
            if (co == NULL)
            {
                Py_DECREF(commands);
                return NULL;
            }
            slice = co;
        }
        else if (ref.type == INLINE_CODE)
        {
            co = Code_Parser_Strip(slice);
            Py_DECREF(slice);
            if (co == NULL)
            {
                Py_DECREF(commands);
                return NULL;
            }
            slice = co;
        }
        co = Code_Class_Compile(slice, ref.type, frame_no++, optimization_level);
        Py_DECREF(slice);
        if (co == NULL)
        {
            Py_DECREF(commands);
            return NULL;
        }

        PyList_Append(commands, co);
        Py_DECREF(co);
        start = ref.raw_start;
    }

    return commands;
}

PyTypeObject Py3hpCode_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.py3hp_code",
        .tp_doc = "",
        .tp_basicsize = sizeof(Py3hpCode_Object) - sizeof(PyObject *[1]),
        .tp_itemsize = sizeof(PyObject *),
        .tp_new = NULL,
        .tp_dealloc = (destructor) Py3hpCode_Dealloc
};