from setuptools import setup

setup(
    name="pyhp-python",
    packages=["pyhp"],
    package_data={"pyhp": ["*.pyi"]},
    package_dir={"pyhp": "pkg"},
    classifiers=[
        "Development Status :: 4 - Beta",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3 :: Only",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Topic :: Software Development :: Interpreters",
        "Topic :: Text Processing :: Markup",
        "Topic :: Text Processing :: Markup :: HTML",
        "Typing :: Typed"
    ],
    version="0.0.0rc0"
)
