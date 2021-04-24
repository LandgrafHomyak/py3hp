#include <Python.h>
#include <structmember.h>

#define min(A, B) (((A) > (B))?(B):(A))

PyTypeObject *StringIO_Type;

PyObject *print_open = NULL; /* print(""" */
PyObject *slash_quote = NULL; /* \" */
PyObject *quote = NULL; /* " */
PyObject *slash_slash = NULL; /* \\ */
PyObject *slash = NULL; /* \ */
PyObject *print_close = NULL; /* """)\n */
PyObject *new_line = NULL; /* \n */

#define debug(NAME, PY_O) (printf("#" NAME " = "),PyObject_Print((PY_O), stdout, Py_PRINT_RAW),printf("\n"))

PyObject *Code_Compile(PyTypeObject *cls, PyObject *source)
{
    PyObject *buffer = NULL;
    PyObject *temp_string = NULL;
    Py_ssize_t i;
    PyObject *_tmp_o;
    PyObject *_tmp_slice;
    Py_ssize_t len;
    Py_ssize_t raw_start;
    Py_ssize_t raw_end;
    Py_ssize_t code_start;
    Py_ssize_t code_end;
    PyObject *lines;
    Py_ssize_t line_i;
    PyObject *line;
    Py_ssize_t chr;
    Py_ssize_t slen;
    Py_ssize_t min_indent;
    PyObject *code_o;


#define cgs(VAR, AS) /* create global string */ if ((VAR) == NULL) {(VAR) = PyUnicode_FromString((AS));if ((VAR) == NULL){return NULL;}}0/* zero for requiring semicolon */
    /* cgs(print_open, "print(\"\"\""); */
    cgs(slash_quote, "\\\"");
    cgs(quote, "\"");
    cgs(slash_slash, "\\\\");
    cgs(slash, "\\");
    /* cgs(print_close, "\"\"\")\n"); */
    cgs(new_line, "\n");
#undef cgs
    buffer = PyObject_CallFunction((PyObject *) StringIO_Type, "");

    if (buffer == NULL)
    {
        return NULL;
    }
    debug("buffer", buffer);

#define cts(AS) /* create temp string */ if ((temp_string = PyUnicode_FromString((AS))) == NULL)
#define wtb(SO) /* write to buffer */ if (PyObject_CallMethod(buffer, "write", "O", (SO)) == NULL)
#define fis(AS, II, DST) /* find in string */ if ((_tmp_o = PyObject_CallMethod(source, "find", "sn", (AS), (II))) != NULL) {(DST) = PyLong_AsSsize_t(_tmp_o);Py_DECREF(_tmp_o);_tmp_o = NULL;} else
#define cs(START, END) /* create slice */ if ((END) == NULL) {_tmp_slice = PyObject_CallFunction((PyObject *) &PySlice_Type, "nO", (START), Py_None);} else {_tmp_slice = PyObject_CallFunction((PyObject *) &PySlice_Type, "nn", (START), (END));} if (_tmp_slice == NULL)
#define gs(SSO) /* get slice */ temp_string = PyObject_GetItem((SSO), _tmp_slice); Py_DECREF(_tmp_slice); _tmp_slice = NULL; if (temp_string == NULL)
#define ris(SO, M, R, DST) /* replace in string */ if (((DST) = PyUnicode_Replace((SO), (M), (R), -1)) == NULL)
#define wwp(SO, DST) /* wrap with print */ if (((DST) = PyUnicode_FromFormat("print(\"\"\"%U\"\"\")\n", (SO))) == NULL)
    cts(
            "import sys\n"
            "import io\n"
            "\n"
            "#sys.stdout = io.StringIO()\n"
            "#sys.stderr = sys.stdout\n"
            "\n"
    )
    {
        Py_DECREF(buffer);
        return NULL;
    }
    wtb(temp_string)
    {
        Py_DECREF(temp_string);
        Py_DECREF(buffer);
        return NULL;
    }
    Py_DECREF(temp_string);

    len = PyUnicode_GET_LENGTH(source);
    debug("source len", PyLong_FromSsize_t(len));
    i = 0;
    raw_start = 0;
    raw_end = raw_start;
    /* don't use print */
    while (i < len)
    {
        if (raw_end != -1)
        {
            fis("<?python3", raw_end, raw_end)
            {
                Py_DECREF(buffer);
                return NULL;
            }
        }
        if (raw_end == -1)
        {
            cs(raw_start, NULL)
            {
                Py_DECREF(buffer);
                return NULL;
            }
            gs(source)
            {
                Py_DECREF(buffer);
                return NULL;
            }
            ris(temp_string, slash, slash_slash, temp_string)
            {

                Py_DECREF(temp_string);
                Py_DECREF(buffer);
                return NULL;
            }

            ris(temp_string, quote, slash_quote, temp_string)
            {

                Py_DECREF(temp_string);
                Py_DECREF(buffer);
                return NULL;
            }

            wwp(temp_string, temp_string)
            {
                Py_DECREF(temp_string);
                Py_DECREF(buffer);
                return NULL;
            }

            wtb(temp_string)
            {
                Py_DECREF(temp_string);
                Py_DECREF(buffer);
                return NULL;
            }
            Py_DECREF(temp_string);
            break;
        }

        code_start = raw_end + sizeof("<?python3");
        if (code_start >= len)
        {
            raw_end = -1;
            continue;
        }

        if (PyUnicode_READ_CHAR(source, code_start) != (Py_UCS4) ' ' && PyUnicode_READ_CHAR(source, code_start) == (Py_UCS4) '\n')
        {
            raw_end = code_start;
            continue;
        }

        fis("?>", code_start, code_end)
        {
            Py_DECREF(buffer);
            return NULL;
        }

        if (code_end == -1)
        {
            raw_end = -1;
            continue;
        }

        cs(raw_start, raw_end)
        {
            Py_DECREF(buffer);
            return NULL;
        }
        gs(source)
        {
            Py_DECREF(buffer);
            return NULL;
        }
        ris(temp_string, slash, slash_slash, temp_string)
        {

            Py_DECREF(temp_string);
            Py_DECREF(buffer);
            return NULL;
        }

        ris(temp_string, quote, slash_quote, temp_string)
        {

            Py_DECREF(temp_string);
            Py_DECREF(buffer);
            return NULL;
        }

        wwp(temp_string, temp_string)
        {
            Py_DECREF(temp_string);
            Py_DECREF(buffer);
            return NULL;
        }

        wtb(temp_string)
        {
            Py_DECREF(temp_string);
            Py_DECREF(buffer);
            return NULL;
        }
        Py_DECREF(temp_string);

        raw_start = code_end + sizeof("?>");
        raw_end = raw_start;

        cs(code_start, code_end)
        {
            Py_DECREF(buffer);
            return NULL;
        }
        gs(source)
        {
            Py_DECREF(buffer);
            return NULL;
        }

        debug("code block", temp_string);

        lines = PyUnicode_Splitlines(temp_string, 1);
        debug("code lines", lines);

        Py_DECREF(temp_string);
        if (lines == NULL)
        {
            Py_DECREF(buffer);
        }

        min_indent = -1;
        for (line_i = PyList_GET_SIZE(lines) - 1; line_i > 0 /* first line doesn't checks */; line_i--)
        {
            line = PyList_GET_ITEM(lines, line_i);
            slen = PyUnicode_GET_LENGTH(line);
            for (chr = 0; chr < slen; chr++)
            {
                if (PyUnicode_READ_CHAR(line, chr) != (Py_UCS4) ' ')
                {
                    if (PyUnicode_READ_CHAR(line, chr) != (Py_UCS4) '\n')
                    {
                        if (min_indent == -1)
                        {
                            min_indent = chr;
                        }
                        else
                        {
                            min_indent = min(min_indent, chr);
                        }
                    }
                    break;
                }
            }
            /* if line contains only spaces (is empty), it hasn't indent */
        }

        /* fit first line to common indent */
        /* first line olways without indent because before it no blocked statements */
        /* if (min_indent > 0)
        {
            temp_string = PyUnicode_New(min_indent, (Py_UCS4) ' ');
            if (PyUnicode_Fill(temp_string, 0, min_indent, (Py_UCS4) ' ') == -1)
            {
                Py_DECREF(lines);
                Py_DECREF(buffer);
                return NULL;
            }
            wtb(temp_string)
            {
                Py_DECREF(temp_string);
                Py_DECREF(lines);
                Py_DECREF(buffer);
                return NULL;
            }
            Py_DECREF(temp_string);
        }
        */
        /* strip  first line */
        line = PyList_GET_ITEM(lines, 0);
        slen = PyUnicode_GET_LENGTH(line);
        for (chr = 0; chr < slen; chr++)
        {
            debug("first line pos", PyLong_FromSsize_t(chr));
            if (PyUnicode_READ_CHAR(line, chr) != (Py_UCS4) ' ')
            {
                break;
            }
        }
        if (chr < slen - ((PyUnicode_READ_CHAR(line, slen - 1) == (Py_UCS4) '\n') ? 1 : 0))
        {
            cs(chr, NULL)
            {
                Py_DECREF(lines);
                Py_DECREF(buffer);
                return NULL;
            }
            debug("first line slice", _tmp_slice);
            gs(line)
            {
                Py_DECREF(lines);
                Py_DECREF(buffer);
                return NULL;
            }
            debug("cutted first line", temp_string);
            wtb(temp_string)
            {
                Py_DECREF(temp_string);
                Py_DECREF(lines);
                Py_DECREF(buffer);
                return NULL;
            }
            Py_DECREF(temp_string);
        }
        debug("indent", PyLong_FromSsize_t(min_indent));
        for (line_i = 1; line_i < PyList_GET_SIZE(lines); line_i++)
        {
            line = PyList_GET_ITEM(lines, line_i);
            /* if line[min_indent:] == "" that means that string was empty and don't useful */
            /*
            slen = PyUnicode_GET_LENGTH(line);
            for (chr = 0; chr < slen; chr++)
            {
                if (PyUnicode_READ_CHAR(line, chr) != (Py_UCS4) ' ')
                {
                    if (PyUnicode_READ_CHAR(line, chr) != (Py_UCS4) '\n')
                    {
                    */
            cs(min_indent, NULL)
            {
                Py_DECREF(lines);
                Py_DECREF(buffer);
                return NULL;
            }
            gs(line)
            {

                Py_DECREF(lines);
                Py_DECREF(buffer);
                return NULL;
            }
            wtb(temp_string)
            {
                Py_DECREF(temp_string);
                Py_DECREF(lines);
                Py_DECREF(buffer);
                return NULL;
            }
            Py_DECREF(temp_string);
            /*
            }
            else
            {
                wtb(line)
                {
                    Py_DECREF(lines);
                    Py_DECREF(buffer);
                    return NULL;
                }
            }
            break;
        }
    }
    */
        }

        Py_DECREF(lines);

        wtb(new_line)
        {
            Py_DECREF(buffer);
            return NULL;
        }
    }
#undef cts
#undef wtb
#undef fis
#undef cs
#undef gs
#undef ris
#undef wwp

#define ur(F) /* unused return */ if ((_tmp_o = (F)) != NULL) {Py_DECREF(_tmp_o);} else
    ur(PyObject_CallMethod(buffer, "seek", "n", 0))
    {
        Py_DECREF(buffer);
        return NULL;
    }
    source = PyObject_CallMethod(buffer, "read", "");
    if (source == NULL)
    {
        Py_DECREF(buffer);
        return NULL;
    }

    debug("all parsed", PyObject_Repr(source));
    Py_DECREF(buffer);

    code_o = PyObject_CallFunction(PyObject_GetAttrString(PyImport_AddModule("builtins"), "compile"), "Oss",
                                             source,
                                             "<anonymus>",
                                             "exec"
    );

    if (code_o ==NULL)
    {
        Py_DECREF(source);
        return NULL;
    }
    Py_DECREF(source);

    return code_o;


//    debug("result", PyObject_CallMethod(buffer, "read", ""))
//    return buffer;
}

int main()
{

    Py_Initialize();
    PyObject *module;

    module = PyImport_ImportModule("io");
    PyObject_Print(module, stdout, Py_PRINT_RAW);
    printf("asdfg");
    if (module == NULL)
    {
        return NULL;
    }
    StringIO_Type = (PyTypeObject *) PyObject_GetAttrString(module, "StringIO");
    Py_DECREF(module);
    if (StringIO_Type == NULL)
    {
        return NULL;
    }
    PyObject *s = PyUnicode_FromString("<>");
    Code_Compile(NULL, NULL);
    return Py_FinalizeEx();
}