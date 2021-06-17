#include <Python.h>
#include <PyHP/api.h>

int PyHP_API_CompileCell(PyHP_API_PageCode_Cell *dst, int py_version, PyHP_API_StatementType type, const char *source, Py_ssize_t start, Py_ssize_t len, int optimize)
{
    switch (type)
    {
        case PyHP_API_StatementType_RAW:
            dst->type = PyHP_API_PageCodeType_TEXT;
            dst->value.raw.start = start;
            dst->value.raw.len = len;
            return 0;
        case PyHP_API_StatementType_BLOCK:
            if (py_version == 3)
            {
                return PyHP_API_CompileCell(dst, py_version, PyHP_API_StatementType_BLOCK3, source, start, len, optimize);
            }
        case PyHP_API_StatementType_INLINE:
            if (py_version == 3)
            {
                return PyHP_API_CompileCell(dst, py_version, PyHP_API_StatementType_INLINE3, source, start, len, optimize);
            }
        case PyHP_API_StatementType_BLOCK2:
        case PyHP_API_StatementType_BLOCK1:
        case PyHP_API_StatementType_INLINE2:
        case PyHP_API_StatementType_INLINE1:
            PyErr_Format(PyExc_NotImplementedError, "support of lower python versions not implemented yet");
            return 1;
        case PyHP_API_StatementType_INLINE3:
#if PY_VERSION_HEX >= 0x03020000
            dst->value.code = Py_CompileStringExFlags(source + start, "<py3hp page inline insertion>", Py_eval_input, NULL, optimize);
#else
            dst->value.code = Py_CompileString(source + start, "<py3hp page inline insertion>", Py_eval_input);
#endif
            if (dst->value.code == NULL)
            {
                return 1;
            }
            dst->type = PyHP_API_PageCodeType_EVAL;
            return 0;
        case PyHP_API_StatementType_BLOCK3:
#if PY_VERSION_HEX >= 0x03020000
            dst->value.code = Py_CompileStringExFlags(source + start, "<py3hp page block insertion>", Py_file_input, NULL, optimize);
#else
            dst->value.code = Py_CompileString(source + start, "<py3hp page block insertion>", Py_file_input);
#endif
            if (dst->value.code == NULL)
            {
                return 1;
            }
            dst->type = PyHP_API_PageCodeType_EXEC;
            return 0;
        default:
            PyErr_BadInternalCall();
            return 1;
    }
}

int PyHP_API_Compile(const char *src, Py_ssize_t len, Py_ssize_t *p_slen, PyHP_API_PageCode_Cell **p_statements, Py_ssize_t *p_blen, char **p_buffer, int optimize)
{
    PyHP_API_ParserIteratorState iterator;
    PyHP_API_ParserMatch match;
    char *dst;
    Py_ssize_t new_len;
    Py_ssize_t pos;
    Py_ssize_t st_pos;
    Py_ssize_t st_len;
    PyHP_API_PageCode_Cell *statements;
    Py_ssize_t statements_len;
    Py_ssize_t statements_pos;

    new_len = 0;
    pos = 0;
    dst = PyMem_Malloc(len);
    if (dst == NULL)
    {
        PyErr_NoMemory();
        return 1;
    }

    statements_len = 8;
    statements_pos = 0;
    statements = PyMem_Malloc(sizeof(PyHP_API_PageCode_Cell) * statements_len);
    if (statements == NULL)
    {
        PyMem_Free(dst);
        PyErr_NoMemory();
        return 1;
    }


    PyHP_API_Parser_Init(src, len, &iterator);
    while ((match = PyHP_API_Parser_Next(src, len, &iterator)).type != PyHP_API_StatementType_NONE)
    {
        if (statements_pos >= statements_len)
        {
            statements_len *= 2;
            statements = PyMem_Realloc(statements, sizeof(PyHP_API_PageCode_Cell) * statements_len);
            if (statements == NULL)
            {
                PyMem_Free(dst);
                PyErr_NoMemory();
                return 1;
            }
        }

        if (match.type == PyHP_API_StatementType_RAW)
        {
            st_pos = pos;
            st_len = match.end - match.start;
            memcpy(&(dst[pos]), &(src[match.start]), st_len);
            new_len = pos + st_len;
            pos = new_len;
        }
        else
        {
            st_pos = pos;
            st_len = PyHP_API_AlignCode(&(dst[pos]), src, match.start, match.end - match.start);
            if (st_len < 0)
            {
                return 1;
            }
            dst[st_pos + st_len] = 0; /* for python compiler */
        }

        if (PyHP_API_CompileCell(&(statements[statements_pos++]), PY_MAJOR_VERSION, match.type, dst, st_pos, st_len, optimize) != 0)
        {
            return 1;
        }
    }

    *p_slen = statements_pos;
    *p_statements = statements;
    *p_blen = new_len;
    *p_buffer = dst;

    return 0;
}

void PyHP_Core_ReleaseCompile(PyHP_API_PageCode_Cell *p_statements, char *p_buffer)
{
    PyMem_Free(p_statements);
    PyMem_Free(p_buffer);
}

PyHP_API_PageCode *PyHP_Core_AllocPageCode(Py_ssize_t slen, Py_ssize_t blen)
{
    return PyMem_Malloc(PyHP_API_PageCode_BASESIZE + PyHP_API_PageCode_EXTRASIZE(slen, blen));
}

void PyHP_Core_FreePageCode(PyHP_API_PageCode *self)
{
    PyMem_Free(self);
}

void PyHP_Core_ReleasePageCode(PyHP_API_PageCode *self)
{
    Py_ssize_t i;
    PyHP_API_PageCode_Cell *cell;

    Py_DECREF(self->name);

    i = PyHP_API_PageCode_SLEN(self);
    cell = PyHP_API_PageCode_STATEMENTS(self);
    while (i-- > 0)
    {
        switch (cell->type)
        {

            case PyHP_API_PageCodeType_EXEC:
            case PyHP_API_PageCodeType_EVAL:
                Py_DECREF(cell->value.code);
                break;
        }
        cell++;
    }
}
