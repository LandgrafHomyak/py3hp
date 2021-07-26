import sys
import builtins
import io

from .compiler import compile, page_code, TEXT, EXEC, EVAL


def exec_embed(code, /, fout):
    if isinstance(code, str):
        code = compile(code)
    if not isinstance(code, page_code):
        raise TypeError("Can execute only raw 'str' or precompiled 'pyhp.compiler.page_code' page")

    old_stdout = sys.stdout
    sys.stdout = fout
    try:
        globals = locals = dict()
        for cell in code:
            if cell.type == TEXT:
                fout.write(cell.value)
            elif cell.type == EXEC:
                builtins.exec(cell.value, globals, locals)
            elif cell.type == EVAL:
                fout.write(str(builtins.eval(cell.value, globals, locals)))
    finally:
        sys.stdout = old_stdout


try:
    if sys.version_info < (3, 9):
        import _xxsubinterpreters as interpreters
    else:
        import interpreters


except ImportError:
    pass

def exec(code, /, fout):
    raise NotImplementedError("sub-interprets python api not implemented yet, try use native implementation of PyHP")