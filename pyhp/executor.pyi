from asyncio import Protocol
from types import Union
from typing import Any, Dict, NoReturn

from .compiler import compiler_iterator


class StdoutProxy(Protocol):
    def write(self, s: str, /) -> Any: ...


def exec(source: Union[str, compiler_iterator], /, stdout: StdoutProxy, globals: Dict[str, Any] = ...) -> NoReturn: ...
