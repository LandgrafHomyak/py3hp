#include <Python.h>
#include <structmember.h>

#include <PyHP.h>


int PyHP_Compiler_BindPrepare(PyHP_CompilerState *self, PyHP_PrepareStateWithParent *parent)
{
    self->head.meta_info = &(PyHP_CompilerIterator_Type.im);
    self->prepare_state = parent;
    return 0;
}

int PyHP_Compiler_WrapPrepare(PyHP_CompilerStateWithParent *self, PyHP_PrepareStateWithParent *parent)
{
    if (PyHP_Compiler_BindPrepare(&(self->self), parent) != 0)
    {
        return 1;
    }
    self->self.prepare_state = NULL;
    self->parent = *parent;
    return 0;
}

int PyHP_Compiler_FromPrepare(PyHP_CompilerStateWithParent *self, PyHP_PrepareStateWithParent *parent)
{
    PyHP_PrepareStateWithParent copied;
    if (PyHP_Prepare_Copy(parent, &copied) != 0)
    {
        return 1;
    }
    if (PyHP_Compiler_WrapPrepare(self, &copied) != 0)
    {
        PyHP_Prepare_Free(&copied);
        return 1;
    }
    return 0;
}

int PyHP_Compiler_Copy(PyHP_CompilerStateWithParent *self, PyHP_CompilerStateWithParent *dst)
{
    if (self->self.prepare_state == NULL)
    {
        return PyHP_Compiler_FromPrepare(dst, &(self->parent));
    }
    else
    {
        return PyHP_Compiler_BindPrepare(&(dst->self), self->self.prepare_state);
    }
}

void PyHP_Compiler_Free(PyHP_CompilerStateWithParent *self)
{
    if (self->self.prepare_state == NULL)
    {
        PyHP_Prepare_Free(&(self->parent));
    }
}

int PyHP_Compiler_Next(PyHP_CompilerStateWithParent *self, PyHP_Command *dst)
{
    PyHP_PrepareStateWithParent *ps;
    PyHP_PrepareResult pr;

    if (self->self.prepare_state == NULL)
    {
        ps = &(self->parent);
    }
    else
    {
        ps = self->self.prepare_state;
    }

    switch (PyHP_Prepare_Next(ps, &pr))
    {
        case PyHP_Iterator_NEXT_ERROR:
            return PyHP_Iterator_NEXT_ERROR;
        case PyHP_Iterator_NEXT_END:
            return PyHP_Iterator_NEXT_END;
        case PyHP_Iterator_NEXT_SUCCESSFUL:
            break;
    }

    switch (pr.type)
    {
        case PyHP_StatementType_RAW:
            dst->type = PyHP_CommandType_TEXT;
            dst->value = pr.string;
            return PyHP_Iterator_NEXT_SUCCESSFUL;
        case PyHP_StatementType_INLINE:
            dst->type = PyHP_CommandType_EVAL;
            dst->value = Py_CompileString(pr.string, "<pyhp inline insertion>", Py_eval_input);
            if (dst->value == NULL)
            {
                return PyHP_Iterator_NEXT_ERROR;
            }
            return PyHP_Iterator_NEXT_SUCCESSFUL;
        case PyHP_StatementType_BLOCK:
            dst->type = PyHP_CommandType_EXEC;
            dst->value = Py_CompileString(pr.string, "<pyhp block insertion>", Py_file_input);
            if (dst->value == NULL)
            {
                return PyHP_Iterator_NEXT_ERROR;
            }
            return PyHP_Iterator_NEXT_SUCCESSFUL;
    }
}

static PyHP_CompilerState_Object *PyHP_CompilerIterator_New(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"", NULL};
    PyHP_PrepareState_Object *parent;
    PyHP_CompilerState_Object *self;
    PyHP_CompilerStateWithParent cs;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kw_list, &PyHP_PrepareIterator_Type, &parent))
    {
        return NULL;
    }

    if (PyHP_Compiler_BindPrepare(&(cs.self), &(PyHP_PrepareIterator_DATA(parent))) != 0)
    {
        return NULL;
    }

    self = (PyHP_CompilerState_Object *) PyHP_CompilerIterator_Type.tp.tp_alloc(&(PyHP_CompilerIterator_Type.tp), 0);
    if (self == NULL)
    {
        PyHP_Compiler_Free(&cs);
        PyErr_NoMemory();
        return NULL;
    }
    self->data = cs;
    self->parent = (PyObject *) parent;
    Py_INCREF(parent);

    return self;


}

static void PyHP_CompilerIterator_Dealloc(PyHP_CompilerState_Object *self)
{
    PyHP_Compiler_Free(&(self->data));
    Py_XDECREF(self->parent);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *PyHP_CompilerIterator_Repr(PyHP_CompilerState_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object at %p>",
            Py_TYPE(self)->tp_name,
            self
    );
}


static PyHP_CompilerState_Object *PyHP_CompilerIterator_Iter(PyHP_CompilerState_Object *self)
{
    Py_INCREF(self);
    return self;
}


static PyHP_Command_Object *PyHP_CompilerIterator_Next(PyHP_CompilerState_Object *self)
{
    PyHP_Command_Object *new;
    PyHP_Command cm;

    switch (PyHP_Compiler_Next(&(self->data), &cm))
    {
        case PyHP_Iterator_NEXT_ERROR:
        case PyHP_Iterator_NEXT_END:
            return NULL;
        case PyHP_Iterator_NEXT_SUCCESSFUL:
            break;
    }

    new = (PyHP_Command_Object *) PyHP_Command_Type.tp_alloc(&PyHP_Command_Type, 0);
    if (new == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }
    new->data = cm;

    return new;
}



PyHP_IteratorMeta_Object PyHP_CompilerIterator_Type = {
        {
                PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name = "pyhp.parser.compiler_iterator",
                .tp_basicsize = sizeof(PyHP_CompilerState_Object),
                .tp_new = (newfunc)PyHP_CompilerIterator_New,
                .tp_dealloc = (destructor) PyHP_CompilerIterator_Dealloc,
                .tp_repr = (reprfunc) PyHP_CompilerIterator_Repr,
                .tp_iter = (getiterfunc) PyHP_CompilerIterator_Iter,
                .tp_iternext = (iternextfunc) PyHP_CompilerIterator_Next
        }
};





















