import os
import re
from distutils.core import Extension, setup
from distutils.ccompiler import new_compiler
from distutils.dist import Distribution as OldDistribution
from distutils.command.build import build as old_build
from distutils.command.build_ext import build_ext as old_build_ext
from distutils.sysconfig import customize_compiler
from distutils.util import get_platform

try:
    pass
    # from setuptools.config import read_configuration
except ImportError:
    def read_configuration(filepath, find_others=False, ignore_option_errors=False):
        """Read given configuration file and returns options from it as a dict.

        :param str|unicode filepath: Path to configuration file
            to get options from.

        :param bool find_others: Whether to search for other configuration files
            which could be on in various places.

        :param bool ignore_option_errors: Whether to silently ignore
            options, values of which could not be resolved (e.g. due to exceptions
            in directives such as file:, attr:, etc.).
            If False exceptions are propagated as expected.

        :rtype: dict
        """

import sys


# read_configuration("setup.cfg")

class Distribution(OldDistribution):
    def __init__(self, *args, **kwargs):
        self.shared_libs = None
        super().__init__(*args, **kwargs)


class build(old_build):
    def has_so(self):
        return self.distribution.shared_libs is not None

    sub_commands = [("build_so", has_so)] + old_build.sub_commands


class build_so(old_build_ext):
    def finalize_options(self):
        super().finalize_options()
        self.extensions = self.distribution.shared_libs

    def build_extensions(self):  # this function called from .run()
        for so in self.extensions:
            self.build_so(so)

    def build_extension(self, *args, **kwargs):
        raise NotImplementedError

    def build_so(self, so):
        macros = so.define_macros[:]
        for undef in so.undef_macros:
            macros.append((undef,))

        o = self.compiler.compile(
            sources=so.sources,
            output_dir=self.build_temp,
            macros=macros,
            include_dirs=so.include_dirs,
            debug=self.debug,
            extra_postargs=so.extra_compile_args or [],
            depends=so.depends
        )
        print(o)
        self.compiler.link_shared_lib(
            objects=o,
            output_libname=so.name,
            output_dir="./",
            export_symbols=[],
            libraries=self.get_libraries(so),
            library_dirs=so.library_dirs,
            runtime_library_dirs=so.runtime_library_dirs,
            extra_postargs=so.extra_compile_args or [],
            debug=self.debug,
            build_temp=self.build_temp,
            target_lang="c"
        )


class SharedLib(Extension):
    pass


def parse_def_file_symbols(path):
    with open(path, "rt") as deffileio:
        s = deffileio.read()

    match = re.search(r"(?:^|\n)(?i:EXPORTS)\s*\n", s)
    if match is None:
        return []
    start = match.end(0)

    match = re.search(r"(?:\n\S|$)", s[start:])
    end = start + match.start(0)  # end of string ($) always exists

    symbols = re.findall(r"(?:^|\n)\s+(\S+)(?:\s|\n|$)", s[start:end])
    return symbols


api_so = SharedLib(
    name="PyHP_API",
    sources=[
        "./src/c/api/main.c",
        "./src/c/api/encoding.c",
        "./src/c/api/parser.c",
        "./src/c/api/compiler.c",
        "./src/c/api/executor.c",
    ],
    include_dirs=["./src/c/Include"],
    export_symbols=parse_def_file_symbols("src/c/api/exports.def")
)
core_so = SharedLib(
    name="PyHP_API",
    sources=[
        "./src/c/core/main.c",
        "./src/c/core/parser.c",
        "./src/c/core/compiler.c",
        "./src/c/core/executor.c",
    ],
    include_dirs=["./src/c/Include"],
    export_symbols=parse_def_file_symbols("src/c/core/exports.def")
)

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
    libraries=["PyHP_API"]
)

args = dict(
    name="pyhp",
    ext_modules=[
        meta_ext,
        core_ext
    ],
    ext_package="pyhp",
    packages=["pyhp"],
    # entry_points={"console_scripts": {
    #     "py3hp": "py3hp.interpreter:main"
    # }}
    package_data={"pyhp": []},
    version="0.0.0b0",
    shared_libs=[
        api_so
    ],
    package_dir={"pyhp": "pyhp"},
    distclass=Distribution,
    cmdclass={"build": build, "build_so": build_so}
)

if sys.version_info >= (3, 5):
    args["package_data"]["pyhp"].extend(["py.typed", "__init__.pyi", "core.pyi", "_meta.pyi", "libs/*"])

setup(
    **args,
    libraries=[]
)
