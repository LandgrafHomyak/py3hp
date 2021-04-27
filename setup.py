from distutils.core import Extension, setup

core_ext = Extension(
    name="core",
    sources=[
        "./core/main.c",

        "./core/code.c",
        "./core/code/parser.c",
        "./core/code/class.c",
        "./core/code/iterator.c",
        "./core/code/commandtype.c",
        "./core/code/highlevel.c",
    ],
)

setup(
    name="py3hp",
    ext_modules=[
        core_ext,
    ],

    ext_package="py3hp",
    packages=["py3hp"],
    # entry_points={"console_scripts": {
    #     "py3hp": "py3hp.interpreter:main"
    # }}
    package_data={"py3hp": ["py.typed", "__init__.pyi", "core.pyi"]}
)
