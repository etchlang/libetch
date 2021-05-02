#ifndef ETCH_PARSER_HPP
#define ETCH_PARSER_HPP 1

#include <etch/syntax/types.hpp>
#include <etch/syntax/adapted.hpp>
#include <etch/parser/unit.hpp>
#include <string_view>

namespace etch {
	syntax::unit parse(std::string_view sv);
} // namespace etch

#endif
