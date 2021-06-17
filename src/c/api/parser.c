#include <string.h>

#include <Python.h>
#include <structmember.h>

#include <PyHP/api.h>

static const char open_block_3_tag[] = "<?python3";
static const char open_inline_3_tag[] = "<?3=";
static const char open_block_2_tag[] = "<?python2";
static const char open_inline_2_tag[] = "<?2=";
static const char close_tag[] = "?>";


void PyHP_API_Parser_Init(const char *string, Py_ssize_t len, PyHP_API_ParserIteratorState *state)
{
    state->pos = 0;
    state->index = 0;
}

PyHP_API_ParserMatch PyHP_API_Parser_Next(const char *string, Py_ssize_t len, PyHP_API_ParserIteratorState *state)
{
    Py_ssize_t i;
    PyHP_API_ParserMatch match;
    Py_ssize_t code_end;
    Py_ssize_t newline_pos;

    switch (state->index)
    {
        case 1:
            match.type = PyHP_API_StatementType_RAW;
            match.start = state->raw_start;
            match.end = state->raw_end;
            if (state->code_match.type == PyHP_API_StatementType_NONE)
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
            match.type = PyHP_API_StatementType_NONE;
            return match;
        case 0:
            break;
        default:
            match.type = PyHP_API_StatementType_NONE;
            return match;
    }

    state->code_match.type = PyHP_API_StatementType_NONE;
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
                                    case '1':
                                        state->code_match.type = PyHP_API_StatementType_INLINE1;
                                        break;
                                    case '2':
                                        state->code_match.type = PyHP_API_StatementType_INLINE2;
                                        break;
                                    case '3':
                                        state->code_match.type = PyHP_API_StatementType_INLINE3;
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
                                    case '1':
                                        state->code_match.type = PyHP_API_StatementType_BLOCK1;
                                        break;
                                    case '2':
                                        state->code_match.type = PyHP_API_StatementType_BLOCK2;
                                        break;
                                    case '3':
                                        state->code_match.type = PyHP_API_StatementType_BLOCK3;
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
        state->code_match.type = PyHP_API_StatementType_NONE;
    }
    return PyHP_API_Parser_Next(string, len, state);

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

    return PyHP_API_Parser_Next(string, len, state);
}

Py_ssize_t PyHP_API_AlignCode(char *dst, const char *src, const Py_ssize_t start, const Py_ssize_t len)
{
    Py_ssize_t d_pos; /* position on destination string */
    Py_ssize_t pos; /* position on source string */
    Py_ssize_t i_pos; /* offset position in previous indent */
    Py_ssize_t i_start; /* start position of previous offset (including common part) */
    Py_ssize_t i_common; /* len of common indent */
    Py_ssize_t i_len; /* len of previous indent (including common part) */
    Py_ssize_t c_start; /* start position of current line */
    Py_ssize_t c_line; /* number of current line, for errors formatting */
    src += start;

    /* remove indent from first line (line after <? literal) */
    d_pos = 0;
    for (pos = 0; pos < len && src[pos] != '\n'; pos++)
    {
        if (d_pos != 0 || (src[pos] != ' ' && src[pos] != '\t'))
        {
            dst[d_pos++] = src[pos];
            /* printf("!* %d '%c'\n", pos, src[pos]); */
        }
        /* else
        {
            printf("!_ %d\n", pos);
        } */
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
        /* printf("~ %d\n", pos); */
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
        /* printf("| %d\n", pos); */

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
            /* printf("@~ %d\n", pos); */
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
            /* printf("@_ %d\n", pos); */
        }
        /* check for new indent level */
        while (pos < len && (src[pos] == ' ' || src[pos] == '\t'))
        {
            /*  printf("@+ %d\n", pos); */
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
            /* printf("@* %d '%c'\n", pos, src[pos]); */
            dst[d_pos++] = src[pos];
        }

        c_line++;
    }

    return d_pos;
}
