#ifndef ETCH_PARSER_HPP
#define ETCH_PARSER_HPP 1

#include <etch/ast.hpp>
#include <string_view>

namespace etch {
	ast::expr parse(std::string_view sv);
} // namespace etch

#endif
