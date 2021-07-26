import re

RAW = 1
INLINE = 2
BLOCK = 3

pyhp_pattern = re.compile(r"(?:<\?=([^\n]+?)|<\?python([ \n\t][\s\S]+?))\?>")


class parser_match:
    __slots__ = ("value", "type", "start", "end")

    def __init__(self, value, type, start, end):
        self.value = value
        self.type = type
        self.start = start
        self.end = end


class parser_iterator:
    __slots__ = ("_source", "__pos", "__next")

    def __init__(self, source: str):
        self._source = source
        self.__pos = 0
        self.__next = None

    def __iter__(self):
        return self

    def __next__(self):
        if self.__next is not None:
            next = self.__next
            self.__next = None
            return next

        if self.__pos >= len(self._source):
            raise StopIteration

        search_result = pyhp_pattern.search(self._source, self.__pos)
        if search_result is None:
            search_result = parser_match(value=self._source[self.__pos:], type=RAW, start=self.__pos, end=len(self._source))
            self.__pos = len(self._source)
            return search_result
        else:
            match = parser_match(value=self._source[self.__pos:search_result.start()], type=RAW, start=self.__pos, end=search_result.start())
            self.__pos = search_result.end()
            if search_result.group(1) is not None:
                self.__next = parser_match(value=self._source[search_result.start(1):search_result.end(1)], type=INLINE, start=search_result.start(1), end=search_result.end(1))
            else:
                self.__next = parser_match(value=self._source[search_result.start(2):search_result.end(2)], type=BLOCK, start=search_result.start(2), end=search_result.end(2))
            return match


def parse(source: str, /):
    return parser_iterator(source)


indent_pattern = re.compile("\n([ \t]*)([^\n]+)(?=\n|$)")


def align_code(source, /, start=None, end=None):
    source = source[start:end] + "\n"
    first_line = source[:source.find("\n")].lstrip()
    lines = indent_pattern.findall(source)
    if lines:
        indent = lines[0][0]
        base_indent = len(indent)
        for i, v in enumerate(lines):
            if v[0].startswith(indent) or indent.startswith(v[0]):
                indent = v[0]
                lines[i] = v[0][base_indent:] + v[1]
            else:
                raise IndentationError
        return first_line + "\n" + "\n".join(lines)
    else:
        return first_line
