from setuptools import setup

setup(
    name="pyhp",
    version="0.0+python",
    description="PHP-like HTML preprocessor for Python",
    author="Andrew Golovashevich",
    url="https://LandgrafHomyak.github.io/pyhp/",
    download_url="https://github.com/LandgrafHomyak/pyhp/releases/tag/v0.0+python",
    packages=["pyhp"],
    python_requires=">=3.5, <3.12",
    install_requires=[],
    package_data={
        "pyhp": ["py.typed", "*.pyi"],
    },
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python",
        "Topic :: Software Development",
        "Typing :: Typed"
    ],
    license="",
    license_files=["LICENSE"],
)
