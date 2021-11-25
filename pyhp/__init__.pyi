from collections import Mapping
from io import TextIOBase
from typing import Any, NoReturn


def exec(source: str, output_file: TextIOBase, _globals: Mapping[str, Any] = dict(), /) -> NoReturn: ...

def execs(source: str, _globals: Mapping[str, Any] = dict(), /) -> str: ...

def align_code(source: str, /, start: int = ..., end=...) -> str: ...
