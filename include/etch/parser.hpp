#ifndef ETCH_PARSER_HPP
#define ETCH_PARSER_HPP 1

#include <etch/ast.hpp>
#include <etch/ast_adapted.hpp>
#include <etch/parser/module.hpp>
#include <string_view>

namespace etch {
	ast::module parse(std::string_view sv);
} // namespace etch

#endif
