from distutils.core import Extension, setup

core_ext = Extension(
    name="_py3hp",
    sources=[
        "./_py3hp/py3hp.c",
        # "./_py3hp/code/parser.h",
        "./_py3hp/code/parser.c",
        "./_py3hp/code/class.c",
        "./_py3hp/code/iterator.c",
        "./_py3hp/code/commandtype.c",
        "./_py3hp/code/highlevel.c",
        "./_py3hp/code.c",
        # "./_py3hp/code.c",
    ]
)

setup(
    name="py3hp",
    ext_modules=[
        core_ext,
    ],

    # ext_package="py3hp",
    packages=["py3hp"],
    # entry_points={"console_scripts": {
    #     "py3hp": "py3hp.interpreter:main"
    # }}
    package_data={"py3hp": ["py.typed", "__init__.pyi"]}
)
