#ifndef PY3HP_CODE_PARSER_H
#define PY3HP_CODE_PARSER_H

int Code_Parser_Init(void);

void Code_Parser_Free(void);

typedef struct
{
    int type;
    Py_ssize_t raw_end;
    Py_ssize_t code_start;
    Py_ssize_t code_end;
    Py_ssize_t raw_start;
} code_block_slice;

#define ERROR (-1)
#define NOT_FOUND (0)
#define BLOCK_CODE (1)
#define INLINE_CODE (2)

code_block_slice Code_Parser_NextBlock(PyObject *source, Py_ssize_t start);

PyObject *Code_Parser_AlignIndentO(PyObject *source);

PyObject *Code_Parser_AlignIndent(PyObject *source);

PyObject *Code_Parser_Strip(PyObject *source);
#endif /* PY3HP_CODE_PARSER_H */
