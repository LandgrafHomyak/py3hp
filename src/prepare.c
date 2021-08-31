#include <Python.h>
#include <PyHP.h>

#include "py_functions.h"

int PyHP_Prepare_BindParser(PyHP_PrepareState *self, PyHP_ParserState *parent)
{
    Py_ssize_t buffer_size;

    self->head.meta_info = &(PyHP_PrepareIterator_Type.im);
    self->parser_state = parent;
    if (parent->len < 0)
    {
        buffer_size = PyUnicode_GetLength(parent->string) * 4 + 1;
    }
    else
    {
        buffer_size = parent->len + 1;
    }
    self->prepared_string = PyMem_Malloc(buffer_size);
    if (self->prepared_string == NULL)
    {
        PyErr_NoMemory();
        return 1;
    }
    return 0;
}

int PyHP_Prepare_WrapParser(PyHP_PrepareStateWithParent *self, PyHP_ParserState *parent)
{
    if (PyHP_Prepare_BindParser(&(self->self), parent) != 0)
    {
        return 1;
    }
    self->self.parser_state = NULL;
    self->parent = *parent;
    return 0;
}

int PyHP_Prepare_FromParser(PyHP_PrepareStateWithParent *self, PyHP_ParserState *parent)
{
    PyHP_ParserState copied;
    if (PyHP_Parser_Copy(parent, &copied) != 0)
    {
        return 1;
    }
    if (PyHP_Prepare_WrapParser(self, &copied) != 0)
    {
        PyHP_Parser_Free(&copied);
        return 1;
    }
    return 0;
}

int PyHP_Prepare_Copy(PyHP_PrepareStateWithParent *self, PyHP_PrepareStateWithParent *dst)
{
    if (self->self.parser_state == NULL)
    {
        return PyHP_Prepare_FromParser(dst, &(self->parent));
    }
    else
    {
        return PyHP_Prepare_BindParser(&(dst->self), self->self.parser_state);
    }
}

void PyHP_Prepare_Free(PyHP_PrepareStateWithParent *self)
{
    PyMem_Free(self->self.prepared_string);
    self->self.prepared_string = NULL;
    if (self->self.parser_state == NULL)
    {
        PyHP_Parser_Free(&(self->parent));
    }
}


static void PyHP_PrepareIterator_Dealloc(PyHP_PrepareState_Object *self)
{
    PyHP_Prepare_Free(&(self->data));
    Py_XDECREF(self->parent);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *PyHP_PrepareIterator_Repr(PyHP_ParserIterator_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object at %p>",
            Py_TYPE(self)->tp_name,
            self
    );
}

static PyHP_PrepareState_Object *PyHP_PrepareIterator_Iter(PyHP_PrepareState_Object *self)
{
    Py_INCREF(self);
    return self;
}

static PyHP_PrepareResult_Object *PyHP_PrepareIterator_Next(PyHP_PrepareState_Object *self)
{
    PyHP_PrepareResult_Object *new;
    PyHP_PrepareResult pr;
    PyObject *raw_string;

    switch (PyHP_Prepare_Next(&(self->data), &pr))
    {
        case PyHP_Iterator_NEXT_ERROR:
        case PyHP_Iterator_NEXT_END:
            return NULL;
        case PyHP_Iterator_NEXT_SUCCESSFUL:
            break;
    }

    switch (pr.type)
    {

        case PyHP_StatementType_RAW:
            raw_string = NULL;
            break;
        case PyHP_StatementType_INLINE:
        case PyHP_StatementType_BLOCK:
            raw_string = PyUnicode_FromString(pr.string);
            if (raw_string == NULL)
            {
                return NULL;
            }
            break;
    }

    new = (PyHP_PrepareResult_Object *) PyHP_PrepareResult_Type.tp_alloc(&PyHP_PrepareResult_Type, 0);
    if (new == NULL)
    {
        PyErr_NoMemory();
        Py_XDECREF(raw_string);
        return NULL;
    }
    new->data = pr;
    new->raw_string = raw_string;

    return new;
}

PyHP_IteratorMeta_Object PyHP_PrepareIterator_Type = {
        {
                PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name = "pyhp.parser.prepare_iterator",
                .tp_basicsize = sizeof(PyHP_PrepareState_Object),
                .tp_dealloc = (destructor) PyHP_PrepareIterator_Dealloc,
                .tp_repr = (reprfunc) PyHP_PrepareIterator_Repr,
                .tp_iter = (getiterfunc) PyHP_PrepareIterator_Iter,
                .tp_iternext = (iternextfunc) PyHP_PrepareIterator_Next
        }
};

PyHP_PrepareState_Object *PyHP_Prepare_Func(PyObject *module, PyObject *args)
{
    PyHP_ParserIterator_Object *parent;
    PyHP_PrepareState_Object *self;
    PyHP_PrepareStateWithParent ps;


    if (!PyArg_ParseTuple(args, "O!", &PyHP_ParserIterator_Type, &parent))
    {
        return NULL;
    }

    if (PyHP_Prepare_BindParser(&(ps.self), &(PyHP_ParserIterator_DATA(parent))) != 0)
    {
        return NULL;
    }

    self = (PyHP_PrepareState_Object *) PyHP_PrepareIterator_Type.tp.tp_alloc(&(PyHP_PrepareIterator_Type.tp), 0);
    if (self == NULL)
    {
        PyHP_Prepare_Free(&ps);
        PyErr_NoMemory();
        return NULL;
    }
    self->data = ps;
    self->parent = (PyObject *) parent;
    Py_INCREF(parent);

    return self;
}