#include <string.h>

#include <Python.h>
#include <structmember.h>
#include "py3hp.h"

#include "parser.h"
#include "encoding.h"

static const char open_block_3_tag[] = "<?python3";
static const char open_inline_3_tag[] = "<?3=";
static const char open_block_2_tag[] = "<?python2";
static const char open_inline_2_tag[] = "<?2=";
static const char close_tag[] = "?>";


PY3HP_LOW_API void Py3hp_Core_Parser_Init(const char *string, Py_ssize_t len, Py3hp_Core_ParserIteratorState *state)
{
    state->pos = 0;
    state->index = 0;
}

PY3HP_LOW_API Py3hp_Core_ParserMatch Py3hp_Core_Parser_Next(const char *string, Py_ssize_t len, Py3hp_Core_ParserIteratorState *state)
{
    Py_ssize_t i;
    Py3hp_Core_ParserMatch match;
    Py_ssize_t code_end;
    Py_ssize_t newline_pos;

    switch (state->index)
    {
        case 1:
            match.type = Py3hp_Core_StatementType_RAW;
            match.start = state->raw_start;
            match.end = state->raw_end;
            if (state->code_match.type == Py3hp_Core_StatementType_NONE)
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
            match.type = Py3hp_Core_StatementType_NONE;
            return match;
        case 0:
            break;
        default:
            match.type = Py3hp_Core_StatementType_NONE;
            return match;
    }

    state->code_match.type = Py3hp_Core_StatementType_NONE;
    newline_pos = state->pos - 1;
    for (i = state->pos; i < len; i++)
    {
        switch (string[i])
        {
            case '\n':
                newline_pos = i;
                break;
            case '?':
                if (len - i >= 2 && string[i + 1] == '>')
                {
                    code_end = i;
                    for (i = state->pos; i < code_end; i++)
                    {
                        if (string[i] == '<' /* && len - i >= 2 */ && string[i + 1] == '?') /* check not require because i < code_end <= len - 2 */
                        {
                            if (i > newline_pos && code_end - i >= 4 &&
                                (string[i + 2] == '3' || string[i + 2] == '2') &&
                                string[i + 3] == '='
                                    )
                            {
                                switch (string[i + 2])
                                {
                                    case '2':
                                        state->code_match.type = Py3hp_Core_StatementType_INLINE3;
                                        break;
                                    case '3':
                                        state->code_match.type = Py3hp_Core_StatementType_INLINE2;
                                        break;
                                }
                                state->raw_start = state->pos;
                                state->code_match.start = i + 4;
                                goto found;
                            }
                            if (code_end - i >= 10 &&
                                string[i + 2] == 'p' &&
                                string[i + 3] == 'y' &&
                                string[i + 4] == 't' &&
                                string[i + 5] == 'h' &&
                                string[i + 6] == 'o' &&
                                string[i + 7] == 'n' &&
                                (string[i + 8] == '2' || string[i + 8] == '3') &&
                                (string[i + 9] == ' ' || string[i + 9] == '\n' || string[i + 9] == '\t')
                                    )
                            {
                                switch (string[i + 8])
                                {
                                    case '2':
                                        state->code_match.type = Py3hp_Core_StatementType_BLOCK3;
                                        break;
                                    case '3':
                                        state->code_match.type = Py3hp_Core_StatementType_BLOCK2;
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
        state->code_match.type = Py3hp_Core_StatementType_NONE;
    }
    return Py3hp_Core_Parser_Next(string, len, state);

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

    return Py3hp_Core_Parser_Next(string, len, state);
}

#if PY_VERSION_HEX >= 0x03000000
#elif PY_VERSION_HEX >= 0x02000000
# if PY_VERSION_HEX >= 0x02020000
PY3HP_HIGH_API Py3hp_Core_ParserIterator_Object *Py3hp_Core_OldParser_Func(PyObject *module, PyObject *string)
# elif PY_VERSION_HEX >= 0x02000000
PY3HP_HIGH_API PyObject *Py3hp_Core_OldParser_Func(PyObject *module, PyObject *string)
# endif
{
    PyObject *self;
    Py3hp_Core_ParserIteratorState state;
    Py3hp_Core_ParserMatch match;
    Py_ssize_t len;
# if PY_VERSION_HEX >= 0x02020000
    Py3hp_Core_ParserMatch_Object *object;
# else
    PyObject *value;
    PyObject *object;
# endif
    const char *src;

# if PY_VERSION_HEX >= 0x02020000
    if (!(PyUnicode_Check(string)))
# elif PY_VERSION_HEX >= 0x02010000
    if(!(PyObject_IsInstance(string, &PyUnicode_Type)))
# else
    if(!(PyUnicode_Check(string))) /* not checks for subclasses */
# endif
    {
        PyErr_Format(PyExc_TypeError, "source must be str, not %s", Py_TYPE(string)->tp_name);
    }

    src = Py3hp_Core_EncodeStringRO(string, &len);
    if (src == NULL)
    {
        return NULL;
    }

    Py3hp_Core_Parser_Init(src, len, &state);

    self = PyList_New(0);
    if (self == NULL)
    {
        return NULL;
    }

    while ((match = Py3hp_Core_Parser_Next(src, len, &state)).type != Py3hp_Core_StatementType_NONE)
    {
# if PY_VERSION_HEX >= 0x02020000
        object = (Py3hp_Core_ParserMatch_Object *) (Py3hp_Core_ParserMatch_Type.tp_alloc(&Py3hp_Core_ParserMatch_Type, 0));
        if (object == NULL)
        {
            Py_DECREF(self);
            PyErr_NoMemory();
            return NULL;
        }

        object->meta = match;
        object->value = Py3hp_Core_DecodeString(src, match.start, match.end - match.start);
        if (object == NULL)
        {
            Py_DECREF(object);
            Py_DECREF(self);
            return NULL;
        }
# else
        value = Py3hp_Core_DecodeString(src, match.start, match.end - match.start);
        if (value == NULL)
        {
            Py_DECREF(self);
            return NULL;
        }
        object = Py_BuildValue("iOii", match.type, value, match.start, match.end);
        Py_DECREF(value);
        if (object == NULL)
        {
            Py_DECREF(self);
            return NULL;
        }
# endif
        if (PyList_Append(self, object) != 0)
        {
            Py_DECREF(object);
            Py_DECREF(self);
            return NULL;
        }
    }
    return self;
}
#else
#endif

#if PY_VERSION_HEX >= 0x02020000
PY3HP_HIGH_API Py3hp_Core_ParserIterator_Object *Py3hp_Core_Parser_Func(PyObject *module, PyObject *string)
{
    Py3hp_Core_ParserIterator_Object *self;
    const char *src;
    Py_ssize_t len;

# if PY_VERSION_HEX >= 0x02020000
    if (!(PyUnicode_Check(string)))
# elif PY_VERSION_HEX >= 0x02010000
        if(!(PyObject_IsInstance(string, &PyUnicode_Type)))
# else
    if(!(PyUnicode_Check(string))) /* not checks for subclasses */
# endif
    {
        PyErr_Format(PyExc_TypeError, "source must be str, not %s", Py_TYPE(string)->tp_name);
    }

    src = Py3hp_Core_EncodeStringRO(string, &len);
    if (src == NULL)
    {
        return NULL;
    }

    self = (Py3hp_Core_ParserIterator_Object *) (Py3hp_Core_ParserIterator_Type.tp_alloc(&Py3hp_Core_ParserIterator_Type, len));
    if (self == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    memcpy(self->buffer, src, len);
    Py3hp_Core_Parser_Init(self->buffer, len, &self->state);

    return self;
}

PY3HP_HIGH_API static Py3hp_Core_ParserIterator_Object *Py3hp_Core_ParserIterator_Iter(Py3hp_Core_ParserIterator_Object *self)
{
    Py_INCREF(self);
    return self;
}

PY3HP_HIGH_API static Py3hp_Core_ParserMatch_Object *Py3hp_Core_ParserIterator_Next(Py3hp_Core_ParserIterator_Object *self)
{
    Py3hp_Core_ParserMatch match;
    Py3hp_Core_ParserMatch_Object *object;

    match = Py3hp_Core_Parser_Next(self->buffer, Py_SIZE(self), &(self->state));

    if (match.type == Py3hp_Core_StatementType_NONE)
    {
        return NULL;
    }

    object = (Py3hp_Core_ParserMatch_Object *) (Py3hp_Core_ParserMatch_Type.tp_alloc(&Py3hp_Core_ParserMatch_Type, 0));
    if (object == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    object->meta = match;
    object->value = Py3hp_Core_DecodeString(self->buffer, match.start, match.end - match.start);
    if (object->value == NULL)
    {
        Py_DECREF(object);
        return NULL;
    }

    return object;
}

PY3HP_HIGH_API static void Py3hp_Core_ParserIterator_Dealloc(Py3hp_Core_ParserIterator_Object *self)
{
    Py_TYPE(self)->tp_free(self);
}

PY3HP_HIGH_API static PyObject *Py3hp_Core_ParserIterator_GetSource(Py3hp_Core_ParserIterator_Object *self)
{
    return Py3hp_Core_DecodeString(self->buffer, 0, Py_SIZE(self));
}

PyGetSetDef Py3hp_Core_ParserIterator_GetSet[] = {
        {"_source", (getter) Py3hp_Core_ParserIterator_GetSource, NULL, ""},
        {NULL}
};

PyTypeObject Py3hp_Core_ParserIterator_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.parser_iterator",
        .tp_basicsize = sizeof(Py3hp_Core_ParserIterator_Object) - sizeof(char[1]),
        .tp_itemsize = sizeof(char),
        .tp_dealloc = (destructor) Py3hp_Core_ParserIterator_Dealloc,
# if PY_VERSION_HEX >= 0x02020000
        .tp_iter = (getiterfunc) Py3hp_Core_ParserIterator_Iter,
        .tp_iternext = (iternextfunc) Py3hp_Core_ParserIterator_Next,
        .tp_getset = Py3hp_Core_ParserIterator_GetSet,
# else
# endif

# if PY_VERSION_HEX >= 0x03000000
# elif PY_VERSION_HEX >= 0x02020000
        .tp_flags = Py_TPFLAGS_HAVE_ITER | Py_TPFLAGS_HAVE_CLASS,
# else
# endif
};


PY3HP_HIGH_API static void Py3hp_Core_ParserMatch_Dealloc(Py3hp_Core_ParserMatch_Object *self)
{
    Py_XDECREF(self->value);
    Py_TYPE(self)->tp_free(self);
}

static PyMemberDef Py3hp_Core_ParserMatch_Members[] = {
        {"value", T_OBJECT,   offsetof(Py3hp_Core_ParserMatch_Object, value),      READONLY},
        {"start", T_PYSSIZET, offsetof(Py3hp_Core_ParserMatch_Object, meta.start), READONLY},
        {"end",   T_PYSSIZET, offsetof(Py3hp_Core_ParserMatch_Object, meta.end),   READONLY},
        {NULL}
};

static PyObject *Py3hp_Core_ParserMatch_GetType(Py3hp_Core_ParserMatch_Object *self)
{
    return PyLong_FromLong(self->meta.type);
}

static PyGetSetDef Py3hp_Core_ParserMatch_GetSet[] = {
        {"type", (getter) Py3hp_Core_ParserMatch_GetType, NULL, ""},
        {NULL}
};

PY3HP_HIGH_API static PyObject *Py3hp_Core_ParserMatch_Repr(Py3hp_Core_ParserMatch_Object *self)
{
    return PyUnicode_FromFormat(
            "<%s object; span=(%zd, %zd) type=%d>",
            Py_TYPE(self)->tp_name,
            self->meta.start,
            self->meta.end,
            self->meta.type
    );
}

PY3HP_HIGH_API static PyObject *Py3hp_Core_ParserMatch_Str(Py3hp_Core_ParserMatch_Object *self)
{
    Py_INCREF(self);
    return self->value;
}

PyTypeObject Py3hp_Core_ParserMatch_Type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "py3hp.core.parser_match",
        .tp_basicsize = sizeof(Py3hp_Core_ParserMatch_Object),
        .tp_dealloc = (destructor) Py3hp_Core_ParserMatch_Dealloc,
        .tp_repr = (reprfunc) Py3hp_Core_ParserMatch_Repr,
# if PY_VERSION_HEX >= 0x02020000
        .tp_members = Py3hp_Core_ParserMatch_Members,
        .tp_getset = Py3hp_Core_ParserMatch_GetSet,
# else
# endif
        /* .tp_str = (reprfunc) Py3hp_Core_ParserMatch_Str, */

# if PY_VERSION_HEX >= 0x03000000
# elif PY_VERSION_HEX >= 0x02020000
        .tp_flags = Py_TPFLAGS_HAVE_CLASS,
# else
# endif
};
#else
#endif