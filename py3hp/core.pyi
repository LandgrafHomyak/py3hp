import builtins
from typing import Generic, TypeVar, Union, Literal, overload, Protocol

__BT = TypeVar("__BT")


class py3hp_code_commandtype:
    id: int
    name: str

    def __int__(self) -> int: ...

    def __index__(self) -> int: ...

    def __str__(self) -> str: ...

    def __repr__(self) -> str: ...


py3hp_code_command_raw: py3hp_code_commandtype
py3hp_code_command_eval: py3hp_code_commandtype
py3hp_code_command_exec: py3hp_code_commandtype

__CODE = type(builtins.compile("0", "", "eval"))


class __py3hp_code_command(tuple, Generic[__BT]):
    def __len__(self) -> Literal[2]: ...

    @overload
    def __getitem__(self, item: Literal[0]) -> Union[__BT, __CODE]: ...

    @overload
    def __getitem__(self, item: Literal[1]) -> py3hp_code_commandtype: ...


class __py3hp_code_iterator(Generic[__BT]):
    def __iter__(self) -> __py3hp_code_iterator[__BT]: ...

    def __next__(self) -> __py3hp_code_command[__BT]: ...


class py3hp_code_bytes:
    def __iter__(self) -> __py3hp_code_iterator[bytes]: ...

    def __getitem__(self, item: int) -> __py3hp_code_command[bytes]: ...

    def __len__(self) -> int: ...


class py3hp_code_str(py3hp_code_bytes):
    def __iter__(self) -> __py3hp_code_iterator[str]: ...

    def __getitem__(self, item: int) -> __py3hp_code_command[str]: ...


class __filelike_read(Protocol, Generic[__BT]):
    def read(self) -> __BT: ...


@overload
def compile(
        source: Union[bytes, __filelike_read[bytes]],
        /,
        filename: str = ...,
        *,
        optimize: int = ...
) -> py3hp_code_bytes: ...


@overload
def compile(
        source: Union[str, __filelike_read[str]],
        /,
        filename: str = ...,
        *,
        optimize: int = ...
) -> py3hp_code_str: ...
