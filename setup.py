import os
from distutils.core import Extension, setup
import sys

# with open("./version", "rt") as v_file:
#     version = list(map(str.strip, v_file.read().split("\n")))
#     version[0] = int(version[0])
#     version[1] = int(version[1])
#     version[2] = int(version[2])
#     version[3] = version[3].lower()
#     version[4] = int(version[4])
#     version = tuple(version[:5])
#
# with open("./core/version.inc", "wt") as vi_file:
#     vi_file.write("""
# # define Py3hp_MAJOR_VERSION (""" + str(version[0]) + """)
# # define Py3hp_MINOR_VERSION (""" + str(version[1]) + """)
# # define Py3hp_MICRO_VERSION (""" + str(version[2]) + """)
# # define Py3hp_RELEASE_LEVEL  (0x""" + version[3][0] + """)
# # define Py3hp_RELEASE_SERIAL (""" + str(version[4]) + """)
#
# # define Py3hp_VERSION \"""" + str(version[0]) + """.""" + str(version[1]) + """.""" + str(version[2]) + version[3][0] + str(version[4]) + """\"
# """)

meta_ext = Extension(
    name="_meta",
    sources=[
        "./_meta/main.c",
    ],
    include_dirs=["./core"]
)
core_ext = Extension(
    name="core",
    sources=[
        "./core/main.c",
        "./core/encoding.c",
        "./core/parser.c",
        "./core/compiler.c",
        "./core/streams.c",
        "./core/executor.c"
    ],
)

args = dict(
    name="Python hypertext preprocessor",
    ext_modules=[
        meta_ext,
        core_ext,
    ],
    ext_package="pyhp",
    packages=["pyhp"],
    # entry_points={"console_scripts": {
    #     "py3hp": "py3hp.interpreter:main"
    # }}
    package_data={"pyhp": []},
    version="0.0.0b0"
)

if sys.version_info >= (3, 5):
    args["package_data"]["pyhp"].extend(["py.typed", "__init__.pyi", "core.pyi", "_meta.pyi"])

setup(
    **args
)
