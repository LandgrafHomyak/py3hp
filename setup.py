from distutils.core import Extension, setup

ext = Extension(
    name="ext",
    sources=[
        "./ext/py3hp.c"
    ]
)

setup(
    name="py3hp",
    ext_modules=[
        # ext,
    ],

    ext_package="py3hp",
    packages=["py3hp"],
    # entry_points={"console_scripts": {
    #     "py3hp": "py3hp.interpreter:main"
    # }}
    package_data={"py3hp": ["py.typed", "__init__.pyi"]}
)
