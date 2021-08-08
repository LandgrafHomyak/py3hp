#include <PyHP.h>

#ifndef PyHP_HPP
#define PyHP_HPP

template<typename char_type>
PyHP_ParserMatch PyHP_Parse(PyHP_ParserState *state, const char_type *string, Py_ssize_t len);


template<typename dst_char_type, typename src_char_type>
Py_ssize_t PyHP_AlignCode(dst_char_type *dst, const src_char_type *src, Py_ssize_t start, Py_ssize_t len);

#endif /* PyHP_HPP */
