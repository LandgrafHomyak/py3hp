#include <Python.h>

#include <PyHP.h>
#include <PyHP.hpp>

#include "parser.h"


template<typename char_type>
PyHP_ParserMatch PyHP_Parse(PyHP_ParserState *const state, const char_type *const string, const Py_ssize_t len)
{
    Py_ssize_t i;
    PyHP_ParserMatch match;
    Py_ssize_t code_end;
    Py_ssize_t newline_pos;

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
                            if (i > newline_pos && code_end - i >= 4 && string[i + 2] == '=')
                            {
                                state->code_match.type = PyHP_StatementType_INLINE;
                                state->raw_start = state->pos;
                                state->code_match.start = i + 3;
                                goto found;
                            }
                            else if (
                                    code_end - i >= 10 &&
                                    string[i + 2] == 'p' &&
                                    string[i + 3] == 'y' &&
                                    string[i + 4] == 't' &&
                                    string[i + 5] == 'h' &&
                                    string[i + 6] == 'o' &&
                                    string[i + 7] == 'n' &&
                                    (string[i + 8] == ' ' || string[i + 8] == '\n' || string[i + 8] == '\t')
                                    )
                            {
                                state->code_match.type = PyHP_StatementType_BLOCK;
                                state->raw_start = state->pos;
                                state->code_match.start = i + 8;
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
    return PyHP_Parse<char_type>(state, string, len);

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

    return PyHP_Parse<char_type>(state, string, len);
}

int PyHP_Parser_Next(PyHP_ParserState *self, PyHP_ParserMatch *dst)
{
    if (self->len < 0)
    {
        switch (PyUnicode_KIND(self->string))
        {
            case PyUnicode_1BYTE_KIND:
                *dst = PyHP_Parse<Py_UCS1>(self, PyUnicode_1BYTE_DATA(self->string), PyUnicode_GetLength((PyObject *) (self->string)));
                break;
            case PyUnicode_2BYTE_KIND:
                *dst = PyHP_Parse<Py_UCS2>(self, PyUnicode_2BYTE_DATA(self->string), PyUnicode_GetLength((PyObject *) (self->string)));
                break;
            case PyUnicode_4BYTE_KIND:
                *dst = PyHP_Parse<Py_UCS4>(self, PyUnicode_4BYTE_DATA(self->string), PyUnicode_GetLength((PyObject *) (self->string)));
                break;
            case PyUnicode_WCHAR_KIND:
                *dst = PyHP_Parse<Py_UNICODE>(self, PyUnicode_AS_UNICODE(self->string), PyUnicode_GetLength((PyObject *) (self->string)));
                break;
        }
    }
    else
    {
        *dst = PyHP_Parse<char>(self, (char *) self->string, self->len);
    }
    if (dst->type == PyHP_StatementType_NONE)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
