import sys
from builtins import exec as py_exec, eval as py_eval
import re
from io import StringIO
from threading import Lock

__all__ = ()

_pattern = re.compile(
    r"""
        # <\?(python|=)\s(.*?)\?>
        <\?
            (?:
                (?P<eval>=)
            |
                (?P<exec>python)
            )
            \s
            (?P<source>
                (?(eval)
                    [^\n]*?
                |
                   .*?
                )
            )
        \?>    
    """,
    re.VERBOSE | re.IGNORECASE | re.DOTALL
)

_gil = Lock()


def exec(source: str, output_file, _globals=None, /):
    with _gil:
        old_stdout = sys.stdout
        sys.stdout = output_file
        _globals = _globals or dict()
        for match in _pattern.finditer(source):
            if match.group("eval") is not None:
                sys.stdout.write(
                    py_eval(
                        match.group("source").strip(),
                        _globals
                    )
                )
            elif match.group("exec") is not None:
                py_exec(
                    align_code(match.group("source")),
                    _globals
                )
        sys.stdout = old_stdout


def execs(source: str, _globals, /):
    file = StringIO()
    exec(source, file, _globals)
    file.seek(0)
    return file.read()


_indent_pattern = re.compile("\n([ \t]*)([^\n]+)(?=\n|$)")


def align_code(source, /, start=None, end=None):
    source = source[start:end] + "\n"
    first_line = source[:source.find("\n")].lstrip()
    lines = _indent_pattern.findall(source)
    if lines:
        indent = lines[0][0]
        base_indent = len(indent)
        for i, v in enumerate(lines):
            if v[0].startswith(indent) or indent.startswith(v[0]):
                indent = v[0]
                lines[i] = v[0][base_indent:] + v[1]
            else:
                raise IndentationError
        return first_line + "\n" + "\n".join(lines)
    else:
        return first_line


if __name__ == "__main__":
    pass
