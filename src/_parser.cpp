#include <Python.h>

#include <PyHP.h>
#include "_parser.h"


template<typename char_type>
static PyHP_ParserMatch PyHP_Parser_Next(PyHP_ParserState *const state, const char_type *const string, const Py_ssize_t len)
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
    return PyHP_Parser_Next<char_type>(state, string, len);

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

    return PyHP_Parser_Next<char_type>(state, string, len);
}

int PyHP_Parser_Next(PyHP_ParserState *self, PyHP_ParserMatch *dst)
{
    if (self->len < 0)
    {
        switch (PyUnicode_KIND(self->string))
        {
            case PyUnicode_1BYTE_KIND:
                *dst = PyHP_Parser_Next<Py_UCS1>(self, PyUnicode_1BYTE_DATA(self->string), PyUnicode_GetLength((PyObject *) (self->string)));
                break;
            case PyUnicode_2BYTE_KIND:
                *dst = PyHP_Parser_Next<Py_UCS2>(self, PyUnicode_2BYTE_DATA(self->string), PyUnicode_GetLength((PyObject *) (self->string)));
                break;
            case PyUnicode_4BYTE_KIND:
                *dst = PyHP_Parser_Next<Py_UCS4>(self, PyUnicode_4BYTE_DATA(self->string), PyUnicode_GetLength((PyObject *) (self->string)));
                break;
            case PyUnicode_WCHAR_KIND:
                *dst = PyHP_Parser_Next<Py_UNICODE>(self, PyUnicode_AS_UNICODE(self->string), PyUnicode_GetLength((PyObject *) (self->string)));
                break;
        }
    }
    else
    {
        *dst = PyHP_Parser_Next<char>(self, (char *) self->string, self->len);
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


template<typename dst_char_type, typename src_char_type>
static Py_ssize_t PyHP_AlignCodeS(dst_char_type *const dst, const src_char_type *const src, const Py_ssize_t start, const Py_ssize_t len)
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

    /* remove indent from first line (line after <? literal) */
    d_pos = 0;
    for (; pos < len && src[pos] != '\n'; pos++)
    {
        if (d_pos != 0 || (src[pos] != ' ' && src[pos] != '\t'))
        {
            dst[d_pos++] = src[pos];
        }
    }
    dst[d_pos] = '\n';
    pos++;

    /* calc common indent that based on second line */
    i_start = pos;
    for (; pos < len && src[pos] != '\n'; pos++)
    {
        if (src[pos] != ' ' && src[pos] != '\t')
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
        dst[d_pos++] = '\n';
        pos++;

        c_start = pos;
        i_pos = 0;

        /* check for common indent */
        for (; pos < len && i_pos < i_common; pos++, i_pos++)
        {
            if (src[pos] != src[i_start + i_pos])
            {
                if (src[pos] == '\n')
                {
                    break;
                }
                if (src[pos] == ' ' || src[pos] == '\t')
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
        if (src[pos] == '\n')
        {
            continue;
        }

        /* check for local indent */
        for (; pos < len && i_pos < i_len; pos++, i_pos++)
        {
            if (src[pos] != src[i_start + i_pos])
            {
                if (src[pos] == '\n')
                {
                    break;
                }
                if (src[pos] == ' ' || src[pos] == '\t')
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
            dst[d_pos++] = src[pos];
        }
        /* check for new indent level */
        while (pos < len && (src[pos] == ' ' || src[pos] == '\t'))
        {
            dst[d_pos++] = src[pos++];
        }
        if (src[pos] == '\n')
        {
            continue;
        }
        i_len = pos - c_start;
        i_start = c_start;

        /* copy statement */
        for (; pos < len && src[pos] != '\n'; pos++)
        {
            dst[d_pos++] = src[pos];
        }

        c_line++;
    }
    return d_pos;
}

Py_ssize_t PyHP_AlignCodeS_String(char *dst, const char *src, const Py_ssize_t start, const Py_ssize_t len)
{
    return PyHP_AlignCodeS<char, char>(dst, src, start, len);
}

PyObject *PyHP_AlignCode_Object(PyObject *string, const Py_ssize_t start, const Py_ssize_t len)
{
    void *dst;
    Py_ssize_t new_len;
    PyObject *out;

    switch (PyUnicode_KIND(string))
    {
        case PyUnicode_1BYTE_KIND:
            new_len = sizeof(Py_UCS1) * len;
            break;
        case PyUnicode_2BYTE_KIND:
            new_len = sizeof(Py_UCS2) * len;
            break;
        case PyUnicode_4BYTE_KIND:
            new_len = sizeof(Py_UCS4) * len;
            break;
        case PyUnicode_WCHAR_KIND:
            new_len = sizeof(Py_UNICODE) * len;
            break;
    }

    dst = PyMem_Malloc(new_len);
    if (dst == NULL)
    {
        return PyErr_NoMemory();
    }

    switch (PyUnicode_KIND(string))
    {
        case PyUnicode_1BYTE_KIND:
            new_len = PyHP_AlignCodeS<Py_UCS1, Py_UCS1>((Py_UCS1 *) dst, (Py_UCS1 *) PyUnicode_1BYTE_DATA(string), start, len);
            break;
        case PyUnicode_2BYTE_KIND:
            new_len = PyHP_AlignCodeS<Py_UCS2, Py_UCS2>((Py_UCS2 *) dst, (Py_UCS2 *) PyUnicode_2BYTE_DATA(string), start, len);
            break;
        case PyUnicode_4BYTE_KIND:
            new_len = PyHP_AlignCodeS<Py_UCS4, Py_UCS4>((Py_UCS4 *) dst, (Py_UCS4 *) PyUnicode_4BYTE_DATA(string), start, len);
            break;
        case PyUnicode_WCHAR_KIND:
            new_len = PyHP_AlignCodeS<Py_UNICODE, Py_UNICODE>((Py_UNICODE *) dst, (Py_UNICODE *) PyUnicode_AS_UNICODE(string), start, len);
            break;
    }
    if (new_len < 0)
    {
        PyMem_Free(dst);
        return NULL;
    }
    switch (PyUnicode_KIND(string))
    {
        case PyUnicode_1BYTE_KIND:
        case PyUnicode_2BYTE_KIND:
        case PyUnicode_4BYTE_KIND:
            out = PyUnicode_FromKindAndData(PyUnicode_KIND(string), dst, new_len);
            break;
        case PyUnicode_WCHAR_KIND:
            out = PyUnicode_FromUnicode((Py_UNICODE *) dst, new_len);
            break;
    }
    PyMem_Free(dst);
    if (out == NULL)
    {
        return NULL;
    }
    return out;
}
