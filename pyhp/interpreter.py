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


class UrlArguments:
    __slots__ = ("args", "kwargs")

    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs

    @classmethod
    def parse(cls, s):
        a = list()
        kw = dict()
        for p in s.split("&"):
            if "=" in p:
                kw.__setitem__(*p.split("=", maxsplit=1))
            else:
                a.append(p)

        return cls(*a, **kw)


def interpret(source, args):
    stdout = sys.stdout
    stderr = sys.stderr
    phpout = StringIO()
    sys.stdout = phpout
    sys.stderr = phpout

    pos = 0

    gv, lv = {"input": disabled_input, "url_arguments": UrlArguments.parse(args)}, dict()

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
        try:
            with SupressErrs(phpout):
                exec(script, gv)
        except SystemExit:
            pass
        pos = match.end()

    sys.stdout = stdout
    sys.stderr = stderr

    phpout.seek(0)
    return phpout.read()


def interpret_as_subprocess(fin_path, args="", py_path=sys.executable, timeout=2):
    return subprocess.check_output([py_path, __file__, fin_path, args], timeout=timeout)


def interpret_from_file(fin_path, args):
    with open(fin_path) as fin:
        source = fin.read()

    return interpret(source, args)


if __name__ == '__main__':
    print(interpret_from_file(sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else ""))
