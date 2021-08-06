#include <Python.h>
#include <structmember.h>

#include <PyHP.h>
#include "parser.h"


static const char open_block_tag[] = "<?python";
static const char open_inline_tag[] = "<?=";
static const char close_tag[] = "?>";


int PyHP_Parser_FromString(PyHP_ParserState *self, const char *string, Py_ssize_t len)
{
    self->head.meta_info = &(PyHP_ParserIterator_Type.im);
    self->pos = 0;
    self->index = 0;
    self->string = (void *) string;
    self->len = len;
    return 0;
}

int PyHP_Parser_FromObject(PyHP_ParserState *self, PyObject *string)
{
    self->head.meta_info = &(PyHP_ParserIterator_Type.im);
    self->pos = 0;
    self->index = 0;
    self->string = (void *) string;
    Py_INCREF(string);
    self->len = -1;
    return 0;
}


void PyHP_Parser_Free(PyHP_ParserState *self)
{
    if (self->len < 0)
    {
        Py_DECREF(self->string);
    }
}

int PyHP_Parser_Copy(PyHP_ParserState *self, PyHP_ParserState *dst)
{
    *dst = *self;
    if (self->len < 0)
    {
        Py_INCREF(self->string);
    }
    dst->head.meta_info = &(PyHP_ParserIterator_Type.im);
    return 0;
}

static PyObject *PyHP_ParserIterator_Repr(PyHP_ParserIterator_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object (pos %zd/%zd) at %p>",
            Py_TYPE(self)->tp_name,
            self->data.pos,
            ((self->data.len < 0) ? PyUnicode_GetLength(self->data.string) : self->data.len),
            self
    );
}

static void PyHP_ParserIterator_Dealloc(PyHP_ParserIterator_Object *self)
{
    PyHP_Parser_Free(&(self->data));
    Py_TYPE(self)->tp_free(self);
}

static PyHP_ParserIterator_Object *PyHP_ParserIterator_Iter(PyHP_ParserIterator_Object *self)
{
    Py_INCREF(self);
    return self;
}

static PyHP_ParserMatch_Object *PyHP_ParserIterator_Next(PyHP_ParserIterator_Object *self)
{
    PyHP_ParserMatch_Object *mo;
    PyHP_ParserMatch m;
    PyObject *v;

    switch (PyHP_Parser_Next(&(self->data), &m))
    {
        case -1:
        case 0:
            return NULL;
        case 1:
            break;
    }

    if (self->data.len < 0)
    {
        v = PyUnicode_Substring(self->data.string, m.start, m.end);
    }
    else
    {
        v = PyUnicode_FromStringAndSize(((char *) (self->data.string)) + m.start, m.end - m.start);
    }
    if (v == NULL)
    {
        return NULL;
    }

    mo = (PyHP_ParserMatch_Object *) PyHP_ParserMatch_Type.tp_alloc(&PyHP_ParserMatch_Type, 0);
    if (mo == NULL)
    {
        Py_DECREF(v);
        PyErr_NoMemory();
        return NULL;
    }
    mo->data = m;
    mo->value = v;

    return mo;
}


PyHP_IteratorMeta_Object PyHP_ParserIterator_Type = {
        {
                PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name = "pyhp.parser.parser_iterator",
                .tp_basicsize = sizeof(PyHP_ParserIterator_Object),
                .tp_dealloc = (destructor) PyHP_ParserIterator_Dealloc,
                .tp_repr = (reprfunc) PyHP_ParserIterator_Repr,
                .tp_iter = (getiterfunc) PyHP_ParserIterator_Iter,
                .tp_iternext = (iternextfunc) PyHP_ParserIterator_Next
        },
        {
                .im_next = (PyHP_Iterator_Next_FuncType) PyHP_Parser_Next,
                .im_free = (PyHP_Iterator_Free_FuncType) PyHP_Parser_Free,
                .im_copy = (PyHP_Iterator_Copy_FuncType) PyHP_Parser_Copy,
        }
};

#if 0
int PyHP_ParserState_Converter(PyObject *src, PyHP_ParserState_X *dst)
{
#if PY_VERSION_HEX >= 0x03010000
    if (src == NULL)
    {
        dst->finalize((PyHP_IteratorHead *) &(dst->data));
    }
#endif

    if (PyUnicode_Check(src))
    {
        if (PyHP_Parser_FromObject(dst, src) != 0)
        {
            return 0;
        }
        else
        {
#if PY_VERSION_HEX >= 0x03010000
            return Py_CLEANUP_SUPPORTED;
#else
            return 1;
#endif
        }
    }
/*
    else if (Py_TYPE(src) == (PyTypeObject *) &PyHP_ParserIterator_Type)
    {
        *dst = ((PyHP_ParserIterator_Object *) (src))->data;
        return 1;
    }
*/
    else
    {
        switch (PyObject_IsInstance(src, (PyObject *) &PyHP_ParserIterator_Type))
        {
            case -1:
                return 0;
            case 0:
                PyErr_Format(
                        PyExc_TypeError,
                        "Can't extract parser state from '%R' object",
                        Py_TYPE(src)
                );
                return 0;
            case 1:
                break;
        }
        if (PyHP_Parser_Copy(dst, &(((PyHP_ParserIterator_Object *) (src))->data)) != 0)
        {
            return 0;
        }

#if PY_VERSION_HEX >= 0x03010000
        return Py_CLEANUP_SUPPORTED;
#else
        return 1;
#endif
    }
}
#endif

PyHP_ParserIterator_Object *PyHP_ParserIterator_Wrap(PyHP_ParserState *src)
{
    PyHP_ParserIterator_Object *self;
    self = (PyHP_ParserIterator_Object *) PyHP_ParserIterator_Type.tp.tp_alloc(&(PyHP_ParserIterator_Type.tp), 0);
    if (self == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }
    self->data = *src;
    return self;
}

#if 0
PyHP_ParserIterator_Object *PyHP_ParserIterator_FromString(const char *string, Py_ssize_t len)
{
    PyHP_ParserState s;
    PyHP_ParserIterator_Object *new;

    if (PyHP_Parser_FromString(&s, string, len) != 0)
    {
        return NULL;
    }

    new = PyHP_ParserIterator_Wrap(&s);
    if (new == NULL)
    {
        PyHP_Parser_Free(&s);
        return NULL;
    }
    return new;
}
#endif

int PyHP_ParserIterator_Converter(PyObject *src, PyHP_ParserIterator_Object **dst)
{
    PyHP_ParserState s;

#if PY_VERSION_HEX >= 0x03010000
    if (src == NULL)
    {
        Py_DECREF(dst);
    }
#endif

    if (Py_TYPE(src) == (PyTypeObject *) &PyHP_ParserIterator_Type)
    {
        *dst = (PyHP_ParserIterator_Object *) src;
        Py_INCREF(src);
#if PY_VERSION_HEX >= 0x03010000
        return Py_CLEANUP_SUPPORTED;
#else
        return 1;
#endif
    }
    else if (PyUnicode_Check(src))
    {
        if (PyHP_Parser_FromObject(&s, src) != 0)
        {
            return 0;
        }
        *dst = PyHP_ParserIterator_Wrap(&s);
        if (*dst == NULL)
        {
            PyHP_Parser_Free(&s);
            return 0;
        }
        else
        {
#if PY_VERSION_HEX >= 0x03010000
            return Py_CLEANUP_SUPPORTED;
#else
            return 1;
#endif
        }
    }
    else
    {
        PyErr_Format(
                PyExc_TypeError,
                "Can't convert '%s' object to parser iterator",
                Py_TYPE(src)
        );
        return 0;
    }
#if 0
    switch (PyHP_ParserState_Converter(src, &s))
    {
        case 0:
            return 0;
        case 1:
#if PY_VERSION_HEX >= 0x03010000
            PyErr_Format(
                    PyExc_RuntimeError,
                    "'PyHP_ParserState_Converter' returned value without cleanup support; input type '%R'",
                    Py_TYPE(src)
            );
            return 0;
        case Py_CLEANUP_SUPPORTED:
#endif
            break;
    }
    *dst = PyHP_ParserIterator_Embed(&s);
    if (*dst == NULL)
    {
        PyHP_Parser_Free(&s);
        return 0;
    }

#if PY_VERSION_HEX >= 0x03010000
    return Py_CLEANUP_SUPPORTED;
#else
    return 1;
#endif
#endif
}


#if 0
PyHP_ParserIterator_Object *PyHP_ParserIterator_Wrap(PyHP_ParserState *src)
{
    PyHP_ParserIterator_Object *self;
    PyHP_ParserState s;

    if (PyHP_Parser_Copy(src, &s) != 0)
    {
        return NULL;
    }

    self = PyHP_ParserIterator_Embed(&s);
    if (self == NULL)
    {
        PyHP_Parser_Free(&s);
    }
    return self;
}
#endif

PyHP_ParserIterator_Object *PyHP_Parser_Func(PyObject *module, PyObject *args)
{
    PyObject *string;
    PyHP_ParserState s;
    PyHP_ParserIterator_Object *o;

    if (!PyArg_ParseTuple(args, "U", &string))
    {
        return NULL;
    }

    if (PyHP_Parser_FromObject(&s, string) != 0)
    {
        return NULL;
    }

    o = PyHP_ParserIterator_Wrap(&s);
    if (o == NULL)
    {
        PyHP_Parser_Free(&s);
        return NULL;
    }


    return o;


}