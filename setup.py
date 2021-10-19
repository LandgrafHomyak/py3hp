from skbuild import setup

setup(
    name="pyhp",
    packages=["pyhp"],
    package_data={"pyhp": ["py.typed", "*.pyi"]},
    version="0.0.0b2"
)
