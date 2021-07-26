from .compiler import compile
from .executor import exec_embed


def main():
    import sys
    import os.path

    if len(sys.argv) < 2:
        print("No input files given")

    for path in sys.argv[1:]:
        path = os.path.abspath(path)
        outfilename = os.path.abspath(os.path.splitext(path)[0] + ".html")
        print(f"'{path}' -> '{outfilename}'")
        with open(path, "rt") as infile, open(outfilename, "wt") as outfile:
            exec_embed(infile.read(), outfile)
