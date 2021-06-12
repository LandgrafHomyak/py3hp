import os
from distutils.ccompiler import new_compiler

for idir in os.listdir("C:/Python/Include"):
    c = new_compiler()
    c.add_include_dir("C:/Python/Include/" + idir)
    c.compile(["core/main.c", "core/encoding.c", "core/parser.c"], output_dir="build")