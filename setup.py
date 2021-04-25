from setuptools import Extension, setup

ext = Extension(
    name="_py3hp",
    sources=[
        "./_py3hp/py3hp.c",
        # "./_py3hp/code/parser.h",
        "./_py3hp/code/parser.c",
        "./_py3hp/code/class.c",
        # "./_py3hp/code.c",
    ]
)

setup(
    name="py3hp",
    ext_modules=[
        ext,
    ],

    # ext_package="py3hp",
    packages=["py3hp"],
    # entry_points={"console_scripts": {
    #     "py3hp": "py3hp.interpreter:main"
    # }}
    package_data={"py3hp": ["py.typed", "__init__.pyi"]}
)
