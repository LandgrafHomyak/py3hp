from distutils.core import Extension, setup
import sys

core_ext = Extension(
    name="core",
    sources=[
        "./core/main.c",
        "./core/encoding.c",
        "./core/parser.c"
    ],
)

args = dict(name="py3hp",
            ext_modules=[
                core_ext,
            ],
            ext_package="py3hp",
            packages=["py3hp"],
            # entry_points={"console_scripts": {
            #     "py3hp": "py3hp.interpreter:main"
            # }}
            package_data={"py3hp": []}
            )

if sys.version_info >= (3, 5):
    args["package_data"]["py3hp"].extend(["py.typed", "__init__.pyi", "core.pyi"])

setup(
    **args
)
