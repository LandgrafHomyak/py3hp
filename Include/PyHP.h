#ifndef PyHP_H
#define PyHP_H
/*

const char *PyHP_EncodeStringRO(PyObject *object, Py_ssize_t *p_len);

char *PyHP_EncodeString(PyObject *object, Py_ssize_t *p_len);

PyObject *PyHP_DecodeString(const char *raw, Py_ssize_t start, Py_ssize_t len);

typedef enum
{
    PyHP_StatementType_NONE = 0,
    PyHP_StatementType_RAW = 1,
    PyHP_StatementType_INLINE = 2,
    PyHP_StatementType_BLOCK = 3,
    PyHP_StatementType_INLINE3 = 4,
    PyHP_StatementType_BLOCK3 = 5,
    PyHP_StatementType_INLINE2 = 6,
    PyHP_StatementType_BLOCK2 = 7,
    PyHP_StatementType_INLINE1 = 8,
    PyHP_StatementType_BLOCK1 = 9,
} PyHP_StatementType;

typedef struct
{
    PyHP_StatementType type;
    Py_ssize_t start;
    Py_ssize_t end;
} PyHP_ParserMatch;

typedef struct
{
    Py_ssize_t pos;
    Py_ssize_t raw_start;
    Py_ssize_t raw_end;
    PyHP_ParserMatch code_match;
    signed int index;
} PyHP_ParserIteratorState;

void PyHP_Parser_Init(const char *string, Py_ssize_t len, PyHP_ParserIteratorState *state);

PyHP_ParserMatch PyHP_Parser_Next(const char *string, Py_ssize_t len, PyHP_ParserIteratorState *state);

Py_ssize_t PyHP_AlignCode(char *dst, const char *src, Py_ssize_t start, Py_ssize_t len);


PyObject *PyHP_AlignCode_Func(PyObject *module, PyObject *string);


typedef struct
{
    PyObject_VAR_HEAD
    PyHP_ParserIteratorState state;
    char buffer[1];
} PyHP_ParserIterator_Object;

typedef struct
{
    PyObject_HEAD
    PyObject *value;
    PyHP_ParserMatch meta;
} PyHP_ParserMatch_Object;

PyHP_ParserIterator_Object *PyHP_Parser_Func(PyObject *module, PyObject *string);

extern PyTypeObject PyHP_ParserIterator_Type;
extern PyTypeObject PyHP_ParserMatch_Type;

typedef enum
{
    PyHP_PageCodeType_TEXT = 1,
    PyHP_PageCodeType_EXEC = 2,
    PyHP_PageCodeType_EVAL = 3,
} PyHP_PageCodeType;


typedef struct
{
    PyHP_PageCodeType type;
    union
    {
        PyObject *code;
        struct
        {
            Py_ssize_t start;
            Py_ssize_t len;
        } raw;
    } value;
} PyHP_PageCode_Cell;

int PyHP_CompileCell(PyHP_PageCode_Cell *dst, int py_version, PyHP_StatementType type, const char *source, Py_ssize_t start, Py_ssize_t len, int optimize);

int PyHP_Compile(const char *src, Py_ssize_t len, Py_ssize_t *p_slen, PyHP_PageCode_Cell **p_statements, Py_ssize_t *p_blen, char **p_buffer, int optimize);

void PyHP_ReleaseCompile(PyHP_PageCode_Cell *p_statements, char *p_buffer);


typedef struct
{
    PyObject *name;
    Py_ssize_t buffer_len;
    Py_ssize_t statements_count;
    PyHP_PageCode_Cell statements[1];
    const char buffer[1];
} PyHP_PageCode;

PyHP_PageCode *PyHP_AllocPageCode(Py_ssize_t slen, Py_ssize_t blen);

void PyHP_FreePageCode(PyHP_PageCode *self);

void PyHP_ReleasePageCode(PyHP_PageCode *self);


#define PyHP_PageCode_BASESIZE (sizeof(PyHP_PageCode) - sizeof(PyHP_PageCode_Cell[1]) - sizeof(const char[1]))
#define PyHP_PageCode_EXTRASIZE(SLEN, BLEN) (sizeof(PyHP_PageCode_Cell) * (SLEN) + sizeof(const char) * (BLEN))
#define PyHP_PageCode_BLEN(PAGECODE) (((PyHP_PageCode *)(PAGECODE))->buffer_len)
#define PyHP_PageCode_BUFFER(PAGECODE) ((const char *)(((PyHP_PageCode *)(PAGECODE))->statements + PyHP_PageCode_SLEN(PAGECODE)))
#define PyHP_PageCode_SLEN(PAGECODE) (((PyHP_PageCode *)(PAGECODE))->statements_count)
#define PyHP_PageCode_STATEMENTS(PAGECODE) (((PyHP_PageCode *)(PAGECODE))->statements)


typedef struct
{
    PyObject_VAR_HEAD
    PyHP_PageCode data;
} PyHP_PageCode_Object;

typedef struct
{
    PyObject_HEAD
    PyHP_PageCode_Object *data;
    Py_ssize_t pos;
} PyHP_PageCodeIterator_Object;

PyHP_PageCode_Object *PyHP_Compile_Func(PyObject *module, PyObject *args, PyObject *kwargs);

extern PyTypeObject PyHP_PageCode_Type;
extern PyTypeObject PyHP_PageCodeIterator_Type;


typedef struct
{
    PyThreadState *parent;
    PyThreadState *interpreter;
    PyHP_PageCode *code;
    PyObject *o_stdin;
    PyObject *o_stdout;
    PyObject *o_stderr;
    int exit_code;
} PyHP_Task;

int PyHP_ExecEmbed(PyHP_PageCode *code, PyObject *globals);

int PyHP_Exec(PyHP_Task *meta, PyObject *globals);

typedef struct
{
    PyObject_HEAD
    PyHP_Task data;
} PyHP_Task_Object;

extern PyTypeObject PyHP_Task_Type;

PyObject *PyHP_ExecEmbed_Func(PyObject *module, PyObject *raw_code);

PyObject *PyHP_Exec_Func(PyObject *module, PyObject *args, PyObject *kwargs);

*/

#include "PyHP/parser.h"


#endif /* PyHP_H */
