import sys
import re
from copy import deepcopy
from io import StringIO
# from py3hp.ext import input as disabled_input
import subprocess

dynamic_block = re.compile(r"<\?python3\s*\n([\s\S]*?)\n\s*\?>")
indent = re.compile(r"(?<=\n)( *)(?=[^ \n])")


def disabled_input(prompt=None):
    pass


class SupressErrs:
    def __init__(self, io):
        self.io = io
    def __enter__(self):
        pass

    def __exit__(self, exc_type, exc_val, exc_tb):
        if exc_type is not None:
            self.io.write(f"{exc_type.__name__}: {str(exc_val)}")
        return True


def interpret(source):
    stdout = sys.stdout
    stderr = sys.stderr
    phpout = StringIO()
    sys.stdout = phpout
    sys.stderr = phpout

    pos = 0

    gv, lv = {"input": disabled_input}, dict()

    # modules = {"sys": deepcopy(sys.modules["sys"])}

    while pos < len(source):
        match = dynamic_block.search(source, pos=pos)

        if match is None:
            phpout.write(source[pos:])
            break

        phpout.write(source[pos:match.start()])

        script = "\n" + match.group(1)
        minimal_indent = len(min(
            indent.findall(script) or ("",),
            key=lambda s: len(s)
        ))

        script = re.sub(r"(?<=\n)" + " " * minimal_indent, "", script)

        with SupressErrs(phpout):
            exec(script, gv)
        pos = match.end()

    sys.stdout = stdout
    sys.stderr = stderr

    phpout.seek(0)
    return phpout.read()


def interpret_as_subprocess(fin_path, py_path=sys.executable, timeout=2):
    return subprocess.check_output([py_path, __file__, fin_path], timeout=timeout)


def interpret_from_file(fin_path):
    with open(fin_path) as fin:
        source = fin.read()

    return interpret(source)


if __name__ == '__main__':
    print(interpret_from_file(sys.argv[1]))
