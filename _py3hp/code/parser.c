#include <Python.h>
#include "parser.h"
#include "commandtype.h"

static char entry_tag[] = "<?";
static char open_block_3_tag[] = "<?python3";
static char open_inline_3_tag[] = "<?3=";
static char open_block_2_tag[] = "<?python2";
static char open_inline_2_tag[] = "<?2=";
static char close_tag[] = "?>";

code_block_slice Code_Parser_NextBlock(char *string, Py_ssize_t start, Py_ssize_t len)
{
    code_block_slice result;
#define type (result.type)
#define raw_end (result.raw_end)
#define code_start (result.code_start)
#define code_end (result.code_end)
#define raw_start (result.raw_start)
    Py_ssize_t entry_point;
    Py_ssize_t i;

    

    for (i = start; i < len; i++)
    {
        if (string[i] == entry_tag[0])
        {
            entry_point = i;
            
        }
        else
        {
            continue;
        }

        if (++i < len)
        {
            if (string[i] != entry_tag[1])
            {
                continue;
            }
        }
        else
        {
            type = Py3hpCode_Command_NOT_FOUND;
            return result;
        }

#define match(SUBSTR, TYPE) \
        for (i = entry_point + sizeof(entry_tag) - 1; i < len && i - entry_point < sizeof((SUBSTR)) - 1 && string[i] == (SUBSTR)[i - entry_point]; i++) ;\
        if (i - entry_point == sizeof((SUBSTR)) - 1)\
        {\
            raw_end = entry_point;\
            code_start = i;\
            type = (TYPE);\
            break;\
        }

        match(open_inline_3_tag, Py3hpCode_Command_EVAL)
        /* match(open_inline_2_tag, Py3hpCode_Command_EVAL) */

        match(open_block_3_tag, Py3hpCode_Command_EXEC)
        /* match(open_block_2_tag, Py3hpCode_Command_EXEC) */

#undef match
    }

    if (i >= len)
    {
        type = Py3hpCode_Command_NOT_FOUND;
        return result;
    }

    for (; i < len; i++)
    {
        if (string[i] != close_tag[0])
        {
            continue;
        }

        entry_point = i++;
        for (; i < len && i - entry_point < sizeof(close_tag) - 1 && string[i] == close_tag[i - entry_point]; i++)
        {}
        if (i - entry_point == sizeof(close_tag) - 1)
        {
            code_end = entry_point;
            raw_start = i;
            break;
        }
    }

    return result;
#undef type
#undef raw_end
#undef code_start
#undef code_end
#undef raw_start
}

Py_ssize_t Code_Parser_AlignIndent(char *src, char *dst, Py_ssize_t code_start, Py_ssize_t code_end, Py_ssize_t tab_width)
{
    Py_ssize_t i;
    Py_ssize_t minimal_indent;
    Py_ssize_t current_indent;

    Py_ssize_t first_line_start;
    Py_ssize_t first_line_end;

    Py_ssize_t line_start;

    Py_ssize_t new_length;

    first_line_end = -1;
    for (i = code_start; i < code_end; i++)
    {
        if (src[i] != ' ' && src[i] != '\t')
        {
            first_line_start = i;
            break;
        }
    }
    for (; i < code_end; i++)
    {
        if (src[i] == '\n')
        {
            first_line_end = i;
            break;
        }
    }
    if (first_line_end == -1)
    {
        memcpy(dst, src + first_line_start, code_end - first_line_start);
        return code_end - first_line_start;
    }
    else
    {
        memcpy(dst, src + first_line_start, first_line_end - first_line_start + 1);
        new_length = first_line_end - first_line_start + 1;
    }
    i++;

    minimal_indent = code_end - code_start + 2;
    for (; i < code_end; i++) /* per line */
    {
        line_start = i;
        current_indent = 0;
        for (; i < code_end; i++)
        {
            if (src[i] == '\n' || src[i] != ' ' && src[i] != '\t')
            {
                break;
            }
            current_indent += (src[i] == '\t') ? tab_width : 1;
        }
        if (src[i] == '\n')
        {
            continue;
        }

        minimal_indent = (current_indent < minimal_indent) ? current_indent : minimal_indent;
        for (; i < code_end; i++)
        {
            if (src[i] == '\n')
            {
                break;
            }
        }
    }

    for (i = first_line_end + 1; i < code_end; i++) /* per line */
    {

        current_indent = 0;
        for (; i < code_end; i++)
        {
            if (src[i] == '\n')
            {
                break;
            }
            if (current_indent == minimal_indent)
            {
                break;
            }
            else if (current_indent > minimal_indent)
            {
                PyErr_Format(
                        PyExc_RuntimeError,
                        "minimal indent was calc wrong: current=%zd, minimal=%zd",
                        current_indent,
                        minimal_indent
                );
                return -1;
            }
            current_indent += (src[i] == '\t') ? tab_width : 1;
        }


        if (src[i] == '\n')
        {

            continue;
        }
        line_start = i;
        for (; i < code_end; i++)
        {
            if (src[i] == '\n')
            {
                break;
            }
        }


        memcpy(dst + new_length, src + line_start, i - line_start);
        new_length += i - line_start;
        if (src[i] == '\n')
        {
            dst[new_length++] = '\n';
        }
    }

    return new_length;
}
