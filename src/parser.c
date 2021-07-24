#include <stddef.h>

#include <Python.h>
#include <structmember.h>

#include <PyHP.h>

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

PyHP_ParserMatch PyHP_Parser_Next(PyHP_ParserIteratorState *const state, const void *const data, const Py_ssize_t len, const int kind)
{
    Py_ssize_t i;
    PyHP_ParserMatch match;
    Py_ssize_t code_end;
    Py_ssize_t newline_pos;

#define get(I) (PyUnicode_READ(kind, data, (I)))

    switch (state->index)
    {
        case 1:
            match.type = PyHP_StatementType_RAW;
            match.start = state->raw_start;
            match.end = state->raw_end;
            if (state->code_match.type == PyHP_StatementType_NONE)
            {
                state->index = -1;
            }
            else
            {
                state->index = 2;
            }
            return match;
        case 2:
            state->index = 0;
            return state->code_match;
        case -1:
            match.type = PyHP_StatementType_NONE;
            return match;
        case 0:
            break;
        default:
            match.type = PyHP_StatementType_NONE;
            return match;
    }

    state->code_match.type = PyHP_StatementType_NONE;
    newline_pos = state->pos - 1;
    for (i = state->pos; i < len; i++)
    {
        switch (get(i))
        {
            case '\n':
                newline_pos = i;
                break;
            case '?':
                if (len - i >= 2 && get(i + 1) == '>')
                {
                    code_end = i;
                    for (i = state->pos; i < code_end; i++)
                    {
                        if (get(i) == '<' /* && len - i >= 2 */ && get(i + 1) == '?') /* check not require because i < code_end <= len - 2 */
                        {
                            if (i > newline_pos && code_end - i >= 4 &&
                                (get(i + 2) == '3' || get(i + 2) == '2') &&
                                get(i + 3) == '='
                                    )
                            {
                                switch (get(i + 2))
                                {
                                    case '1':
                                        state->code_match.type = PyHP_StatementType_INLINE1;
                                        break;
                                    case '2':
                                        state->code_match.type = PyHP_StatementType_INLINE2;
                                        break;
                                    case '3':
                                        state->code_match.type = PyHP_StatementType_INLINE3;
                                        break;
                                }
                                state->raw_start = state->pos;
                                state->code_match.start = i + 4;
                                goto found;
                            }
                            if (code_end - i >= 10 &&
                                get(i + 2) == 'p' &&
                                get(i + 3) == 'y' &&
                                get(i + 4) == 't' &&
                                get(i + 5) == 'h' &&
                                get(i + 6) == 'o' &&
                                get(i + 7) == 'n' &&
                                (get(i + 8) == '2' || get(i + 8) == '3') &&
                                (get(i + 9) == ' ' || get(i + 9) == '\n' || get(i + 9) == '\t')
                                    )
                            {
                                switch (get(i + 8))
                                {
                                    case '1':
                                        state->code_match.type = PyHP_StatementType_BLOCK1;
                                        break;
                                    case '2':
                                        state->code_match.type = PyHP_StatementType_BLOCK2;
                                        break;
                                    case '3':
                                        state->code_match.type = PyHP_StatementType_BLOCK3;
                                        break;
                                }
                                state->raw_start = state->pos;
                                state->code_match.start = i + 9;
                                goto found;
                            }
                        }
                    }
                }
                break;
        }
    }

    if (state->pos == len)
    {
        state->index = -1;
    }
    else
    {
        state->index = 1;
        state->raw_start = state->pos;
        state->raw_end = len;
        state->code_match.type = PyHP_StatementType_NONE;
    }
    return PyHP_Parser_Next(state, data, len, kind);

    found:
    state->pos = code_end + 2;
    state->raw_end = i;
    state->code_match.end = code_end;

    if (state->raw_start == state->raw_end)
    {
        state->index = 2;
    }
    else
    {
        state->index = 1;
    }

    return PyHP_Parser_Next(state, data, len, kind);
#undef get
}

Py_ssize_t PyHP_AlignCodeS(void *dst, const void *src, const Py_ssize_t start, const Py_ssize_t len, int kind)
{
    Py_ssize_t d_pos; /* position on destination string */
    Py_ssize_t pos; /* position on source string */
    Py_ssize_t i_pos; /* offset position in previous indent */
    Py_ssize_t i_start; /* start position of previous offset (including common part) */
    Py_ssize_t i_common; /* len of common indent */
    Py_ssize_t i_len; /* len of previous indent (including common part) */
    Py_ssize_t c_start; /* start position of current line */
    Py_ssize_t c_line; /* number of current line, for errors formatting */
    pos = start;

#define geti(I) (PyUnicode_READ(kind, src, (I)))
#define get() (geti(pos))
#define set(C) PyUnicode_WRITE(kind, dst, d_pos++, (C))


    /* remove indent from first line (line after <? literal) */
    d_pos = 0;
    for (; pos < len && get() != '\n'; pos++)
    {
        if (d_pos != 0 || (get() != ' ' && get() != '\t'))
        {
            set(get());
        }
    }
    set('\n');
    d_pos--;
    pos++;

    /* calc common indent that based on second line */
    i_start = pos;
    for (; pos < len && get() != '\n'; pos++)
    {
        if (get() != ' ' && get() != '\t')
        {
            break;
        }
    }
    i_common = pos - i_start;
    i_len = i_common;
    pos = i_start - 1;

    /* remove ident from other lines */
    c_line = 2;
    while (pos < len)
    {
        /* loop for first (and each other) line lefts pos on newline char (\n), at end of code its not needn't */
        set('\n');
        pos++;

        c_start = pos;
        i_pos = 0;

        /* check for common indent */
        for (; pos < len && i_pos < i_common; pos++, i_pos++)
        {
            if (get() != geti(i_start + i_pos))
            {
                if (get() == '\n')
                {
                    break;
                }
                if (get() == ' ' || get() == '\t')
                {
                    PyErr_Format(
                            PyExc_ValueError,
                            "inconsistent use of tabs and spaces in indentation (line %zd, pos %zd)",
                            c_line,
                            pos - c_start
                    );
                }
                else
                {
                    PyErr_Format(
                            PyExc_ValueError,
                            "too small common indent (line %zd, pos %zd)",
                            c_line,
                            pos - c_start
                    );
                }
                return -1;
            }
        }
        if (get() == '\n')
        {
            continue;
        }

        /* check for local indent */
        for (; pos < len && i_pos < i_len; pos++, i_pos++)
        {
            if (get() != geti(i_start + i_pos))
            {
                if (get() == '\n')
                {
                    break;
                }
                if (get() == ' ' || get() == '\t')
                {
                    PyErr_Format(
                            PyExc_ValueError,
                            "inconsistent use of tabs and spaces in indentation (line %zd, pos %zd)",
                            c_line,
                            pos - c_start
                    );
                    return 1;
                }
                else
                {
                    break;
                }
            }
            set(get());
        }
        /* check for new indent level */
        while (pos < len && (get() == ' ' || get() == '\t'))
        {
            set(get());
            pos++;
        }
        if (get() == '\n')
        {
            continue;
        }
        i_len = pos - c_start;
        i_start = c_start;

        /* copy statement */
        for (; pos < len && get() != '\n'; pos++)
        {
            set(get());
        }

        c_line++;
    }
    return d_pos;
}

PyObject *PyHP_AlignCode(PyObject *string, const Py_ssize_t start, const Py_ssize_t len)
{
    void *dst;
    Py_ssize_t new_len;
    PyObject *out;

    dst = PyMem_Malloc(PyUnicode_GET_DATA_SIZE(string));
    if (dst == NULL)
    {
        return PyErr_NoMemory();
    }


    new_len = PyHP_AlignCodeS(dst, PyUnicode_DATA(string), start, len, PyUnicode_KIND(string));
    if (new_len < 0)
    {
        PyMem_Free(dst);
        return NULL;
    }

    out = PyUnicode_FromKindAndData(PyUnicode_KIND(string), dst, new_len);
    PyMem_Free(dst);
    if (out == NULL)
    {
        return NULL;
    }
    return out;
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


    return PyHP_AlignCode(string, start, len);
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
        .tp_name = "py3hp.parser.parser_match",
        .tp_basicsize = sizeof(PyHP_ParserMatch_Object),
        .tp_dealloc = (destructor) PyHP_ParserMatch_Dealloc,
        .tp_repr = (reprfunc) PyHP_ParserMatch_Repr,
        .tp_members = PyHP_ParserMatch_Members,
        .tp_getset = PyHP_ParserMatch_GetSet,
/* .tp_str = (reprfunc) PyHP_ParserMatch_Str, */
};