def align_code(source: str, /, start: int = ..., end: int = ...) -> str: ...


class parser_match:
    @property
    def start(self) -> int: ...

    @property
    def end(self) -> int: ...

    @property
    def type(self) -> int: ...

    @property
    def value(self) -> str: ...


class parser_iterator:
    @property
    def _source(self) -> str: ...

    def __init__(self) -> parser_iterator: ...

    def __next__(self) -> parser_match: ...


def parse(source: str, /) -> parser_iterator: ...
