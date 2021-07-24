from skbuild import setup

setup(
    name="pyhp",
    packages=["pyhp"],
    package_dir={"pyhp": "pkg"},
    package_data={"pyhp": ["py.typed", "*.pyi"]}
)
