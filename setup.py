from setuptools import setup

setup(
    name="pyhp-python",
    packages=["pyhp"],
    package_data={"pyhp": ["py.typed", "__init__.pyi"]},
    package_dir={"pyhp": "pkg"},
    version="0.0.0"
)
