#include <Python.h>

#include <PyHP.h>
#include <PyHP.hpp>

#include "align_code.h"


template<typename dst_char_type, typename src_char_type>
Py_ssize_t PyHP_AlignCode(dst_char_type *const dst, const src_char_type *const src, const Py_ssize_t start, const Py_ssize_t len)
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

PyObject *PyHP_AlignCode_Func(PyObject *module, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"", "start", "len", NULL};
    PyObject *string;
    void *dst;
    Py_ssize_t new_len;
    Py_ssize_t start = 0;
    Py_ssize_t len = PY_SSIZE_T_MAX;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "U|nn", kw_list, &string, &start, &len))
    {
        return NULL;
    }

    if (len < 0)
    {
        PyErr_Format(
                PyExc_ValueError,
                "length must be non-negative"
        );
        return NULL;
    }

    if (start < 0)
    {
        start += PyUnicode_GetLength(string);
    }
    if (start < 0 || start >= PyUnicode_GetLength(string))
    {
        PyErr_Format(
                PyExc_ValueError,
                "string index out of range"
        );
        return NULL;
    }

    if (start + len >= PyUnicode_GetLength(string))
    {
        len = PyUnicode_GetLength(string) - start;
    }

    switch (PyUnicode_KIND(string))
    {
        case PyUnicode_1BYTE_KIND:
            new_len = sizeof(Py_UCS1) * PyUnicode_GetLength(string);
            break;
        case PyUnicode_2BYTE_KIND:
            new_len = sizeof(Py_UCS2) * PyUnicode_GetLength(string);
            break;
        case PyUnicode_4BYTE_KIND:
            new_len = sizeof(Py_UCS4) * PyUnicode_GetLength(string);
            break;
        case PyUnicode_WCHAR_KIND:
            new_len = sizeof(Py_UNICODE) * PyUnicode_GetLength(string);
            break;
    }
    dst = PyMem_Malloc(new_len);
    if (dst == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }
    switch (PyUnicode_KIND(string))
    {
        case PyUnicode_1BYTE_KIND:
            new_len = PyHP_AlignCode<Py_UCS1, Py_UCS1>((Py_UCS1 *) dst, PyUnicode_1BYTE_DATA(string), start, len);
            break;
        case PyUnicode_2BYTE_KIND:
            new_len = PyHP_AlignCode<Py_UCS2, Py_UCS2>((Py_UCS2 *) dst, PyUnicode_2BYTE_DATA(string), start, len);
            break;
        case PyUnicode_4BYTE_KIND:
            new_len = PyHP_AlignCode<Py_UCS4, Py_UCS4>((Py_UCS4 *) dst, PyUnicode_4BYTE_DATA(string), start, len);
            break;
        case PyUnicode_WCHAR_KIND:
            new_len = PyHP_AlignCode<Py_UNICODE, Py_UNICODE>((Py_UNICODE *) dst, PyUnicode_AS_UNICODE(string), start, len);
            break;
    }

    if (new_len < 0)
    {
        PyMem_Free(dst);
        return NULL;
    }

    string = PyUnicode_FromKindAndData(PyUnicode_KIND(string), dst, new_len);
    PyMem_Free(dst);
    if (string == NULL)
    {
        return NULL;
    }

    return string;
}