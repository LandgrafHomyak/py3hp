from types import CodeType
from typing import Union

from .parser import parser_iterator


class page_code_cell:
    @property
    def type(self) -> int: ...

    @property
    def value(self) -> Union[str, CodeType]: ...


class page_code_iterator:
    def __iter__(self) -> page_code_iterator: ...

    def __next__(self) -> page_code_cell: ...


class page_code:
    def __iter__(self) -> page_code_iterator: ...


def compile(parser: Union[str, parser_iterator], /, optimize: int = ...) -> page_code:
