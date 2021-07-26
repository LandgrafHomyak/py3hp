#include <Python.h>
#include <structmember.h>

#include <PyHP.h>
#include <PyHP/modules.h>

static const char open_block_3_tag[] = "<?python3";
static const char open_inline_3_tag[] = "<?3=";
static const char open_block_2_tag[] = "<?python2";
static const char open_inline_2_tag[] = "<?2=";
static const char close_tag[] = "?>";


void PyHP_Parser_Init(PyHP_ParserIteratorState *const state)
{
    state->pos = 0;
    state->index = 0;
}

PyObject *PyHP_AlignCode_Func(PyObject *module, PyObject *args, PyObject *kwargs)
{
    static char *kwlist[] = {"", "start", "end", NULL};
    PyObject *string;
    Py_ssize_t start;
    Py_ssize_t end;
    Py_ssize_t len;

    start = 0;
    end = PY_SSIZE_T_MAX;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "U|nn:align_code", kwlist, &string, &start, &end))
    {

        return NULL;
    }

    if (end == PY_SSIZE_T_MAX)
    {
        end = PyUnicode_GET_LENGTH(string);
    }
    else if (end != PY_SSIZE_T_MAX && end > PyUnicode_GET_LENGTH(string))
    {
        PyErr_Format(
                PyExc_IndexError,
                "string index out of range ((end=%zd) > %zd)",
                end,
                PyUnicode_GET_LENGTH(string)
        );
        return NULL;
    }
    else if (end < 0)
    {
        end += PyUnicode_GET_LENGTH(string);
        if (end < 0)
        {
            PyErr_Format(
                    PyExc_IndexError,
                    "string index out of range ((end=%zd) < %zd)",
                    end - PyUnicode_GET_LENGTH(string),
                    -PyUnicode_GET_LENGTH(string)
            );
            return NULL;
        }
    }

    if (start < 0)
    {
        start += PyUnicode_GET_LENGTH(string);
        if (start < 0)
        {
            PyErr_Format(
                    PyExc_IndexError,
                    "string index out of range ((start=%zd) < %zd)",
                    start - PyUnicode_GET_LENGTH(string),
                    -PyUnicode_GET_LENGTH(string)
            );
            return NULL;
        }
    }

    if (start > end)
    {
        PyErr_Format(
                PyExc_IndexError,
                "requested string slice [%zd:%zd] has negative length ",
                start,
                end
        );
    }

    len = end - start;


    return PyHP_AlignCode_Object(string, start, len);
}

PyHP_ParserIterator_Object *PyHP_Parser_Func(PyObject *module, PyObject *string)
{
    PyHP_ParserIterator_Object *self;

    if (!(PyUnicode_Check(string)))
    {
        PyErr_Format(PyExc_TypeError, "source must be str, not %s", Py_TYPE(string)->tp_name);
    }


    self = (PyHP_ParserIterator_Object *) (PyHP_ParserIterator_Type.tp_alloc(&PyHP_ParserIterator_Type, 0));
    if (self == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }
    PyHP_Parser_Init(&self->state);
    Py_INCREF(string);
    self->string = string;

    return self;
}

static PyHP_ParserIterator_Object *PyHP_ParserIterator_Iter(PyHP_ParserIterator_Object *self)
{
    Py_INCREF(self);
    return self;
}

static PyHP_ParserMatch_Object *PyHP_ParserIterator_Next(PyHP_ParserIterator_Object *self)
{
    PyHP_ParserMatch match;
    PyHP_ParserMatch_Object *object;

    match = PyHP_Parser_Next_Object(&(self->state), self->string);

    if (match.type == PyHP_StatementType_NONE)
    {
        return NULL;
    }

    object = (PyHP_ParserMatch_Object *) (PyHP_ParserMatch_Type.tp_alloc(&PyHP_ParserMatch_Type, 0));
    if (object == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    object->meta = match;
    object->value = PyUnicode_Substring(self->string, match.start, match.end - match.start);
    if (object->value == NULL)
    {
        Py_DECREF(object);
        return NULL;
    }

    return object;
}

static void PyHP_ParserIterator_Dealloc(PyHP_ParserIterator_Object *self)
{
    Py_DECREF(self->string);
    Py_TYPE(self)->tp_free(self);
}

PyMemberDef PyHP_ParserIterator_Members[] = {
        {"_source", T_OBJECT, offsetof(PyHP_ParserIterator_Object, string), READONLY},
        {NULL}
};


PyTypeObject PyHP_ParserIterator_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "pyhp.parser.parser_iterator",
        .tp_basicsize = sizeof(PyHP_ParserIterator_Object),
        .tp_dealloc = (destructor) PyHP_ParserIterator_Dealloc,
        .tp_iter = (getiterfunc) PyHP_ParserIterator_Iter,
        .tp_iternext = (iternextfunc) PyHP_ParserIterator_Next,
        .tp_members = PyHP_ParserIterator_Members,
};


static void PyHP_ParserMatch_Dealloc(PyHP_ParserMatch_Object *self)
{
    Py_XDECREF(self->value);
    Py_TYPE(self)->tp_free(self);
}

static PyMemberDef PyHP_ParserMatch_Members[] = {
        {"value", T_OBJECT,   offsetof(PyHP_ParserMatch_Object, value),      READONLY},
        {"start", T_PYSSIZET, offsetof(PyHP_ParserMatch_Object, meta.start), READONLY},
        {"end",   T_PYSSIZET, offsetof(PyHP_ParserMatch_Object, meta.end),   READONLY},
        {NULL}
};

static PyObject *PyHP_ParserMatch_GetType(PyHP_ParserMatch_Object *self)
{
    return PyLong_FromLong(self->meta.type);
}

static PyGetSetDef PyHP_ParserMatch_GetSet[] = {
        {"type", (getter) PyHP_ParserMatch_GetType, NULL, ""},
        {NULL}
};

static PyObject *PyHP_ParserMatch_Repr(PyHP_ParserMatch_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object; span=(%zd, %zd) type=%d>",
            Py_TYPE(self)->tp_name,
            self->meta.start,
            self->meta.end,
            self->meta.type
    );
}

static PyObject *PyHP_ParserMatch_Str(PyHP_ParserMatch_Object *self)
{
    Py_INCREF(self);
    return self->value;
}

PyTypeObject PyHP_ParserMatch_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "pyhp.parser.parser_match",
        .tp_basicsize = sizeof(PyHP_ParserMatch_Object),
        .tp_dealloc = (destructor) PyHP_ParserMatch_Dealloc,
        .tp_repr = (reprfunc) PyHP_ParserMatch_Repr,
        .tp_members = PyHP_ParserMatch_Members,
        .tp_getset = PyHP_ParserMatch_GetSet,
/* .tp_str = (reprfunc) PyHP_ParserMatch_Str, */
};

static PyMethodDef PyHPInit_parser_methods[] = {
        {"align_code", (PyCFunction) PyHP_AlignCode_Func, METH_VARARGS | METH_KEYWORDS},
        {"parse",      (PyCFunction) PyHP_Parser_Func,    METH_O},
        {NULL}
};

static PyModuleDef PyHPInit_parser_def = {
        .m_name = "pyhp.parser",
        .m_methods = PyHPInit_parser_methods
};

PyMODINIT_FUNC PyHPInit_parser(void)
{
    PyObject *module;

    if (PyHP_Init() != 0)
    {
        return NULL;
    }

    module = PyModule_Create(&PyHPInit_parser_def);
    if (module == NULL)
    {
        return NULL;
    }

    PyModule_AddObject(module, "parser_iterator", (PyObject *) &PyHP_ParserIterator_Type);
    PyModule_AddObject(module, "parser_match", (PyObject *) &PyHP_ParserMatch_Type);
    return module;
}