import builtins
from .parser import parse, align_code, RAW, INLINE, BLOCK, parser_iterator

TEXT = 1
EXEC = 2
EVAL = 3


class page_code_cell:
    __slots__ = ("type", "value")

    def __init__(self, type, value):
        self.type = type
        self.value = value


class page_code(tuple):
    pass


def compile(parser, /, optimize=-1):
    if isinstance(parser, str):
        parser = parse(parser)
    if not isinstance(parser, parser_iterator):
        raise TypeError("source to be compiled as PyHP must be object of 'str' or 'pyhp.parser.parser_iterator'")

    cells = []
    for match in parser:
        if match.type == RAW:
            cells.append(page_code_cell(TEXT, match.value))
        elif match.type == BLOCK:
            cells.append(page_code_cell(EXEC, builtins.compile(align_code(match.value), "<py3hp page block insertion>", "exec", optimize=optimize)))
        elif match.type == INLINE:
            cells.append(page_code_cell(EVAL, builtins.compile(align_code(match.value), "<py3hp page inline insertion>", "eval", optimize=optimize)))

    return page_code(cells)
