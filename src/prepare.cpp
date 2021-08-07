#include <string.h>

#include <Python.h>

#include <PyHP.h>
#include <PyHP.hpp>


int PyHP_Prepare_Next(PyHP_PrepareStateWithParent *self, PyHP_PrepareResult *dst)
{
    PyHP_ParserState *ps;
    PyHP_ParserMatch pm;
    PyObject *temp;
    Py_ssize_t len;

    if (self->self.parser_state == NULL)
    {
        ps = &(self->parent);
    }
    else
    {
        ps = self->self.parser_state;
    }

    switch (PyHP_Parser_Next(ps, &pm))
    {
        case PyHP_Iterator_NEXT_ERROR:
            return PyHP_Iterator_NEXT_ERROR;
        case PyHP_Iterator_NEXT_END:
            return PyHP_Iterator_NEXT_END;
        case PyHP_Iterator_NEXT_SUCCESSFUL:
            break;
    }

    dst->type = pm.type;
    switch (pm.type)
    {
        case PyHP_StatementType_RAW:
            if (ps->len < 0)
            {
                dst->string = PyUnicode_Substring((PyObject *) (ps->string), pm.start, pm.end );
            }
            else
            {
                dst->string = PyUnicode_FromStringAndSize((char *) (ps->string) + pm.start, pm.end - pm.start);
            }
            if (dst->string == NULL)
            {
                return PyHP_Iterator_NEXT_ERROR;
            }
            return PyHP_Iterator_NEXT_SUCCESSFUL;
        case PyHP_StatementType_INLINE:
        case PyHP_StatementType_BLOCK:
            if (ps->len < 0)
            {
                temp = PyUnicode_Substring((PyObject *) (ps->string), pm.start, pm.end);
                if (temp == NULL)
                {
                    return PyHP_Iterator_NEXT_ERROR;
                }
                dst->string = (void *) PyUnicode_AsUTF8AndSize(temp, &len);
                if (dst->string == NULL)
                {
                    Py_DECREF(temp);
                    return PyHP_Iterator_NEXT_ERROR;
                }
                len = PyHP_AlignCode<char, char>((char *) (self->self.prepared_string), (char *) (dst->string), 0, len);
                Py_DECREF(temp);
            }
            else
            {
                len = PyHP_AlignCode<char, char>((char *) (self->self.prepared_string), (char *) (ps->string), pm.start, pm.end - pm.start);
            }
            if (len < 0)
            {
                return PyHP_Iterator_NEXT_ERROR;
            }
            ((char *) (self->self.prepared_string))[len] = 0;
            dst->string = self->self.prepared_string;
            return PyHP_Iterator_NEXT_SUCCESSFUL;
    }
}