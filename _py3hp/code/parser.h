#include <Python.h>
#include "commandtype.h"

#ifndef PY3HP_CODE_PARSER_H
#define PY3HP_CODE_PARSER_H

int Code_Parser_Init(void);

void Code_Parser_Free(void);


typedef struct
{
    Py3hpCode_Command_Type type;
    Py_ssize_t raw_end;
    Py_ssize_t code_start;
    Py_ssize_t code_end;
    Py_ssize_t raw_start;
} code_block_slice;

code_block_slice Code_Parser_NextBlock(char *string, Py_ssize_t start, Py_ssize_t len);

Py_ssize_t Code_Parser_AlignIndent(char *src, char *dst, Py_ssize_t code_start, Py_ssize_t code_end, Py_ssize_t tab_width);

#endif /* PY3HP_CODE_PARSER_H */
