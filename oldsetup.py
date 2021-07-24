import sys
import os
import re
import shutil
from distutils.core import Extension, setup
from distutils.ccompiler import new_compiler
from distutils.msvccompiler import MSVCCompiler
from distutils.dist import Distribution as OldDistribution
from distutils.command.build import build as old_build
from distutils.command.build_ext import build_ext as old_build_ext
from distutils.command.build_clib import build_clib as old_build_clib
from distutils.cmd import Command

if "--github-actions" in sys.argv:
    sys.argv.pop(sys.argv.index("--github-actions"))
    GITHUB_ACTIONS = True
else:
    GITHUB_ACTIONS = False

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


# read_configuration("setup.cfg")

class Distribution(OldDistribution):
    def __init__(self, *args, **kwargs):
        self.shared_libs = None
        super().__init__(*args, **kwargs)


class build(old_build):
    def has_so(self):
        return self.distribution.shared_libs is not None

    sub_commands = [("gen_py_pkg", lambda *args, **kwargs: True), ("build_so", has_so)] + old_build.sub_commands


class build_clib(old_build_clib):
    def finalize_options(self):
        super().finalize_options()
        old_build_ext.finalize_options(self)


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
        if GITHUB_ACTIONS:
            print("\u001b[36m", end="")
        print(" Building", ("\u001b[4m" if GITHUB_ACTIONS else "") + so.name + ("\u001b[0m" if GITHUB_ACTIONS else ""), "library")
        if GITHUB_ACTIONS:
            print("\u001b[0m", end="")

        macros = so.define_macros[:]
        for undef in so.undef_macros:
            macros.append((undef,))

        if GITHUB_ACTIONS:
            print("::group::Compiling", so.name, "objects")
        else:
            print("  Compiling objects")
        try:
            o = self.compiler.compile(
                sources=so.sources,
                output_dir=self.build_temp,
                macros=macros,
                include_dirs=so.include_dirs,
                debug=self.debug,
                extra_postargs=so.extra_compile_args or [],
                depends=so.depends
            )
        finally:
            if GITHUB_ACTIONS:
                print("::endgroup::")

        if GITHUB_ACTIONS:
            print("::group::Creating", so.name, "static lib")
        else:
            print("  Creating static lib")
        try:
            self.compiler.create_static_lib(
                objects=o,
                output_libname=so.name,
                output_dir="./generated_package",
                debug=self.debug,
                target_lang="c"
            )
        finally:
            if GITHUB_ACTIONS:
                print("::endgroup::")

        if GITHUB_ACTIONS:
            print("::group::Linking", so.name, "shared lib")
        else:
            print("  Linking shared lib")
        try:
            self.compiler.link_shared_lib(
                objects=o,
                output_libname=so.name,
                output_dir="./generated_package",
                export_symbols=so.export_symbols,
                libraries=self.get_libraries(so),
                library_dirs=so.library_dirs,
                runtime_library_dirs=so.runtime_library_dirs,
                extra_postargs=so.extra_compile_args or [],
                debug=self.debug,
                build_temp=self.build_temp,
                target_lang="c"
            )
        finally:
            if GITHUB_ACTIONS:
                print("::endgroup::")

        if GITHUB_ACTIONS:
            print("\u001b[32m", end="")
        print(" " + ("\u001b[4m" if GITHUB_ACTIONS else "") + so.name + ("\u001b[0m\u001b[32m" if GITHUB_ACTIONS else ""), "built successful")
        if GITHUB_ACTIONS:
            print("\u001b[0m", end="")


class SharedLib(Extension):
    pass


class gen_py_pkg(Command):
    def initialize_options(self):
        pass

    def finalize_options(self):
        pass

    def run(self):
        try:
            shutil.rmtree("generated_package")
            if GITHUB_ACTIONS:
                print("\u001b[31m", end="")
            print(" Cleaned generation directory")
            if GITHUB_ACTIONS:
                print("\u001b[0m", end="")
        except:
            pass
        os.mkdir("generated_package")
        print(" Created generation directory")
        current = sys.version_info
        print(" Current python version info:", ("\u001b[33m\u001b[4m" if GITHUB_ACTIONS else "") + str(current) + ("\u001b[0m" if GITHUB_ACTIONS else ""))

        for dir, lib in sorted((z for z in map(self.parse, os.listdir("_src/py")) if z[1] < current), key=lambda _z: _z[0]):
            if GITHUB_ACTIONS:
                print("::group::Processing version point", ("\u001b[33m\u001b[4m" if GITHUB_ACTIONS else "") + "v" + dir + ("\u001b[0m" if GITHUB_ACTIONS else ""))
            else:
                print("  Processing version point", "v" + dir)

            try:
                for file in os.listdir("./src/py/" + dir):
                    print("  Copying file", ("\u001b[34m" if GITHUB_ACTIONS else "") + file + ("\u001b[0m" if GITHUB_ACTIONS else ""))
                    shutil.copyfile("./src/py/" + dir + "/" + file, "generated_package/" + file)
            finally:
                if GITHUB_ACTIONS:
                    print("::endgroup::")

    @staticmethod
    def parse(s):
        return s, tuple(map(int, s.split("."))) + ("final", 0)


import cmake
import os
os.system("cmake --target libPyHP")

core_ext = Extension(
    name="core",
    sources=[
        "src/modules/core.c"
    ],
    libraries=["PyHP"]
)

args = dict(
    name="pyhp",
    ext_modules=[
        core_ext
    ],
    ext_package="pyhp",
    packages=["pyhp"],
    package_data={"pyhp": ["*"]},
    version="0.0.1",
    package_dir={"pyhp": "generated_package"},
    distclass=Distribution,
    setup_requires=[
        "cmake>=3.21.0"
    ]
)

setup(
    **args
)
