from io import TextIOBase
from typing import Union

from .compiler import page_code


def exec_embed(code: Union[str, page_code], /, fout: TextIOBase): ...
