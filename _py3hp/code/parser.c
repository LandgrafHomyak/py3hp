#include <Python.h>
#include "parser.h"

#ifndef NULL
# define NULL ((void *)0)
#endif
static char open_block_tag[] = "<?python3";
static char open_inline_tag[] = "<?3=";
static char close_tag[] = "?>";
static char new_line[] = "\n";

static PyObject *open_block_tag_o = NULL;
static PyObject *open_inline_tag_o = NULL;
static PyObject *close_tag_o = NULL;
static PyObject *new_line_o = NULL;
static PyObject *empty_string_o = NULL;

int Code_Parser_Init(void)
{
    if ((open_block_tag_o = PyUnicode_FromString(open_block_tag)) == NULL)
    {
        return -1;
    }

    if ((open_inline_tag_o = PyUnicode_FromString(open_inline_tag)) == NULL)
    {
        return -1;
    }

    if ((close_tag_o = PyUnicode_FromString(close_tag)) == NULL)
    {
        return -1;
    }

    if ((new_line_o = PyUnicode_FromString(new_line)) == NULL)
    {
        return -1;
    }
#if PY_VERSION_HEX >= 0x03030000
    if ((empty_string_o = PyUnicode_New(0, 0)) == NULL)
#else
        if ((empty_string_o = PyUnicode_FromString("")) == NULL)
#endif
    {
        return -1;
    }
    return 0;
}

void Code_Parser_Free(void)
{
    Py_XDECREF(open_block_tag_o);
    Py_XDECREF(open_inline_tag_o);
    Py_XDECREF(close_tag_o);
    Py_XDECREF(new_line_o);
}


code_block_slice Code_Parser_NextBlock(PyObject *source, Py_ssize_t start)
{
#define return_err() {_result.type = ERROR;return _result;}0
#define return(TYPE, RAW_END, CODE_START, CODE_END, RAW_START) {_result.type = (TYPE);_result.raw_end = (RAW_END);_result.code_start = (CODE_START);_result.code_end = (CODE_END);_result.raw_start = (RAW_START);return _result;}0
#define return_nf() {_result.type = NOT_FOUND;return _result;}0
    code_block_slice _result;

    PyObject *start_pos_o;
    PyObject *end_pos_o;
    Py_ssize_t start_pos;
    Py_ssize_t end_pos;
    Py_UCS4 chr;

    find_end:

    end_pos_o = PyObject_CallMethod(source, "find", "On", close_tag_o, start);
    if (end_pos_o == NULL)
    {
        return_err();
    }
    end_pos = PyLong_AsSsize_t(end_pos_o);
    if (end_pos == -1)
    {
        Py_DECREF(end_pos_o);
        if (PyErr_Occurred())
        {
            return_err();
        }
        else
        {
            return_nf();
        }
    }

    find_start:

    start_pos_o = PyObject_CallMethod(source, "find", "OnO", open_block_tag_o, start, end_pos_o);
    if (start_pos_o == NULL)
    {
        Py_DECREF(end_pos_o);
        return_err();
    }
    start_pos = PyLong_AsSsize_t(start_pos_o);
    Py_DECREF(start_pos_o);
    if (start_pos == -1 && PyErr_Occurred())
    {
        Py_DECREF(end_pos_o);
        return_err();
    }

    if (start_pos != -1)
    {
#if PY_VERSION_HEX >= 0x03030000
        chr = PyUnicode_ReadChar(source, start_pos + sizeof(open_block_tag) - 1);
#else
        chr = PyUnicode_AS_UNICODE(source)[start_pos + sizeof(open_block_tag) - 1];
#endif
        if (chr != (Py_UCS4) ' ' && chr != (Py_UCS4) '\n' && chr != (Py_UCS4) '\t')
        {
            start = start_pos + sizeof(open_block_tag) - 1;
            goto find_start;
        }

        Py_DECREF(end_pos_o);

        return(BLOCK_CODE, start_pos, start_pos + sizeof(open_block_tag) - 1 + 1, end_pos, end_pos + sizeof(close_tag) - 1);
    }
    else
    {
        start_pos_o = PyObject_CallMethod(source, "find", "OnO", open_inline_tag_o, start, end_pos_o);
        Py_DECREF(end_pos_o);
        if (start_pos_o == NULL)
        {
            Py_DECREF(end_pos_o);
            return_err();
        }
        start_pos = PyLong_AsSsize_t(start_pos_o);
        Py_DECREF(start_pos_o);
        if (start_pos == -1)
        {
            if (PyErr_Occurred())
            {
                return_err();
            }
            else
            {
                start = end_pos + sizeof(close_tag) - 1;
                goto find_end;
            }
        }

        return (INLINE_CODE, start_pos, start_pos + sizeof(open_inline_tag) - 1, end_pos, end_pos + sizeof(close_tag) - 1);
    }
}

#ifndef min
# define min(A, B) (((A) < (B))?(A):(B))
# define CUSTOM_MIN
#endif

PyObject *Code_Parser_AlignIndentO(PyObject *source)
{ /* todo reformat code */
    union str_data
    {
        char *bytes;
        Py_UNICODE *unicode;
    };
    union str_data _s_data;
    Py_ssize_t _kind;
    PyObject *dst;
    Py_ssize_t min_indent;
    Py_ssize_t spaces_to_delete = 0;
    Py_ssize_t c;
    Py_ssize_t line_start;
    Py_ssize_t current_indent;
    Py_ssize_t len;
    Py_ssize_t nc;
    union str_data _ns_data;
    Py_ssize_t _nkind;
    Py_ssize_t lines_count;

#if PY_VERSION_HEX >= 0x03030000
# if PY_VERSION_HEX < 0x030c0000 /* will be removed in version 3.12 */
    if (PyUnicode_READY(source) != 0)
    {
        return NULL;
    }
# endif
    _s_data.bytes = PyUnicode_DATA(source);
    switch (PyUnicode_KIND(source))
    {
        case PyUnicode_1BYTE_KIND:
            _kind = 1;
            break;
        case PyUnicode_2BYTE_KIND:
            _kind = 2;
            break;
        case PyUnicode_4BYTE_KIND:
            _kind = 4;
            break;
        default:
            PyErr_Format(
                    PyExc_RuntimeError,
                    "unexpected kind of PyUnicode object"
            );
            return NULL;
    }
# define get(I) PyUnicode_READ_CHAR(source, (I)) /* (*((Py_UCS4 *)(_s_data.bytes + (I) + _kind))) */
#else
    _s_data.unicode = PyUnicode_AS_UNICODE(source);
# define get(I) ((Py_UCS4)(_s_data.unicode[(I)]))
#endif
#define ceq(I, C) /* char equals */ ( (Py_UCS4)get((I)) == (Py_UCS4)(C))


#if PY_VERSION_HEX >= 0x03030000
    len = PyUnicode_GET_LENGTH(source);
#else
    len = PyUnicode_GET_SIZE(source);
#endif
    line_start = 0;
    min_indent = len + 2;
    for (c = 0; c < len; c++)
    {
        if (ceq(c, '\n'))
        {
            spaces_to_delete = c;
            c++;
            line_start = c;
            break;
        }
        if (!ceq(c, ' ') && !ceq(c, '\t'))
        {
            spaces_to_delete = c;
            for (; c < len && !ceq(c, '\n'); c++)
            {}
            c++;
            line_start = c;
            break;
        }
    }
    lines_count = 0;
    current_indent = 0;
    while (c < len)
    {
        if (ceq(c, '\n'))
        {
            spaces_to_delete += c - line_start;
            line_start = ++c;
            current_indent = 0;
            continue;
        }
        if (!ceq(c, ' ') && !ceq(c, '\t'))
        {

            lines_count++;
            for (; c < len && !ceq(c, '\n'); c++)
            {}
            min_indent = min(min_indent, current_indent);
            line_start = ++c;
            current_indent = 0;
            continue;
        }
        current_indent += ((ceq(c++, '\t')) ? 8 : 1);
    }
    if (c == len)
    {
        spaces_to_delete += c - line_start;
    }
    if (min_indent != len + 2)
    {
        spaces_to_delete += lines_count * min_indent;
    }

    if (spaces_to_delete >= len)
    {
        Py_INCREF(empty_string_o);
        return empty_string_o;
    }
#if PY_VERSION_HEX >= 0x03030000

    dst = PyUnicode_New(len - spaces_to_delete, PyUnicode_MAX_CHAR_VALUE(source));
#else
    dst = PyUnicode_FromUnicode(NULL, len - spaces_to_delete);
#endif
    if (dst == NULL)
    {
        return NULL;
    }
#if PY_VERSION_HEX >= 0x03030000
    _ns_data.bytes = PyUnicode_DATA(dst);
    switch (PyUnicode_KIND(dst))
    {
        case PyUnicode_1BYTE_KIND:
            _nkind = 1;
            break;
        case PyUnicode_2BYTE_KIND:
            _nkind = 2;
            break;
        case PyUnicode_4BYTE_KIND:
            _nkind = 4;
            break;
        default:
            PyErr_Format(
                    PyExc_RuntimeError,
                    "unexpected kind of PyUnicode object"
            );
            return NULL;
    }
# define write(I, C) PyUnicode_WRITE(PyUnicode_KIND(dst), _ns_data.bytes, (I), (C))
# define copy(SS, DS, L) (memcpy(_ns_data.bytes + _kind * (DS), _s_data.bytes + _nkind * (SS), (L) * _kind))
#else
    _ns_data.unicode = PyUnicode_AS_UNICODE(dst);
# define write(I, C) ((_ns_data.unicode[(I)] = (Py_UNICODE)(C)))
# define copy(SS, DS, L) (memcpy(_ns_data.unicode + (DS), _s_data.unicode + (SS), (L) * sizeof(Py_UNICODE)))
#endif

    nc = 0;
    for (c = 0; c < len; c++)
    {
        if (ceq(c, '\n'))
        {
            write(nc++, '\n');
            c++;
            break;
        }
        if (!ceq(c, ' ') && !ceq(c, '\t'))
        {
            line_start = c;
            for (; c < len && !ceq(c, '\n'); c++)
            {}
            if (c < len)
            {
                c++;
            }
            copy(line_start, nc, c - line_start);
            nc += c - line_start;
            break;
        }
    }
    current_indent = 0;
    while (c < len)
    {
        if (ceq(c, '\n'))
        {

            write(nc++, '\n');
            c++;
            current_indent = 0;
            continue;
        }
        if (current_indent == min_indent)
        {

            line_start = c;
            for (; c < len && !ceq(c, '\n'); c++)
            {
                if (ceq(c, '\n'))
                {
                    write(nc++, '\n');
                    c++;
                    break;
                }
                if (!ceq(c, ' ') && !ceq(c, '\t'))
                {
                    for (; c < len && !ceq(c, '\n'); c++)
                    {}
                    if (c < len)
                    {
                        c++;
                    }
                    copy(line_start, nc, c - line_start);
                    nc += c - line_start;
                    break;
                }
            }

            current_indent = 0;
            continue;
        }
        else if (current_indent >= min_indent)
        {
            Py_DECREF(dst);
            PyErr_Format(
                    PyExc_RuntimeError,
                    "min_indent was calc wrong (%zd > %zd)",
                    current_indent,
                    min_indent
            );
            return NULL;
        }
        current_indent += ((ceq(c++, '\t')) ? 8 : 1);
    }


    return dst;
#undef get
#undef ceq
#undef write
#undef copy
}

PyObject *Code_Parser_AlignIndent(PyObject *source)
{
#if PY_VERSION_HEX >= 0x03030000
# define get(SO, I) PyUnicode_READ_CHAR((SO), (I))
#else
# define get(SO, I) ((Py_UCS4)(PyUnicode_AS_UNICODE((SO))[(I)]))
#endif
#define ceq(SO, I, C) ((Py_UCS4)get((SO), (I)) == ((Py_UCS4)(C)))
    PyObject *lines;
    PyObject *line_o;
    PyObject *_slice;
    PyObject *stripped_line;
    Py_ssize_t current_indent;
    Py_ssize_t min_indent;
    Py_ssize_t lines_count;
    Py_ssize_t line_len;
    Py_ssize_t line;
    Py_ssize_t c;
    PyObject *dst;

#if PY_VERSION_HEX >= 0x03030000
    min_indent = PyUnicode_GET_LENGTH(source) + 2;
#else
    min_indent = PyUnicode_GET_SIZE(source) + 2;
#endif

    lines = PyUnicode_Splitlines(source, 0);
    if (lines == NULL)
    {
        return NULL;
    }

    line_o = PyList_GET_ITEM(lines, 0);
    stripped_line = Code_Parser_Strip(line_o);
    if (stripped_line == NULL)
    {
        Py_DECREF(lines);
        return NULL;
    }
    Py_DECREF(line_o);
    PyList_SET_ITEM(lines, 0, stripped_line);

    lines_count = PyList_GET_SIZE(lines);
    for (line = 1; line < lines_count; line++)
    {
        line_o = PyList_GET_ITEM(lines, line);
#if PY_VERSION_HEX >= 0x03030000
        line_len = PyUnicode_GET_LENGTH(line_o);
#else
        line_len = PyUnicode_GET_SIZE(line_o);
#endif
        current_indent = 0;
        for (c = 0; c < line_len; c++)
        {
            if (!ceq(line_o, c, ' ') && !ceq(line_o, c, '\t'))
            {
                min_indent = min(min_indent, current_indent);
                break;
            }
            current_indent += (ceq(line_o, c, '\t')) ? 8 : 1;
        }
        if (c == line_len)
        {
            Py_DECREF(line_o);
            Py_INCREF(empty_string_o);
            PyList_SET_ITEM(lines, line, empty_string_o);
        }
    }


    for (line = 1; line < lines_count; line++)
    {
        line_o = PyList_GET_ITEM(lines, line);
#if PY_VERSION_HEX >= 0x03030000
        line_len = PyUnicode_GET_LENGTH(line_o);
#else
        line_len = PyUnicode_GET_SIZE(line_o);
#endif
        current_indent = 0;
        for (c = 0; c < line_len; c++)
        {
            if (current_indent == min_indent)
            {
#if PY_VERSION_HEX >= 0x03030000
                stripped_line = PyUnicode_Substring(line_o, c, line_len);
                if (stripped_line == NULL)
                {
                    Py_DECREF(lines);
                    return NULL;
                }
                Py_DECREF(line_o);
                PyList_SET_ITEM(lines, line, stripped_line);
#else
                _slice = PyObject_CallFunction((PyObject *) &PySlice_Type, "nO", c, Py_None);
                if (_slice == NULL)
                {
                    Py_DECREF(lines);
                    return NULL;
                }

                stripped_line = PyObject_GetItem(line_o, _slice);
                Py_DECREF(_slice);
                if (stripped_line == NULL)
                {
                    Py_DECREF(lines);
                    return NULL;
                }

                Py_DECREF(line_o);
                PyList_SET_ITEM(lines, line, stripped_line);
#endif
                break;
            }
            else if (current_indent > min_indent)
            {
                Py_DECREF(lines);
                PyErr_Format(
                        PyExc_RuntimeError,
                        "min_indent was calc wrong (%zd > %zd)",
                        current_indent,
                        min_indent
                );
                return NULL;
            }
            current_indent += (ceq(line_o, c, '\t')) ? 8 : 1;
        }
    }
    dst = PyUnicode_Join(new_line_o, lines);
    Py_DECREF(lines);
    if (dst == NULL)
    {
        return NULL;
    }

    return dst;
}

#ifdef CUSTOM_MIN
# undef min
# undef CUSTOM_MIN
#endif

PyObject *Code_Parser_Strip(PyObject *source)
{
    PyObject *new;

    new = PyObject_CallMethod(source, "lstrip", "");

    if (new == NULL)
    {
        return NULL;
    }

    return new;
}