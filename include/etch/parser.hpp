#ifndef ETCH_PARSER_HPP
#define ETCH_PARSER_HPP 1

#include <etch/analysis/types.hpp>
#include <etch/syntax/types.hpp>
#include <etch/syntax/adapted.hpp>
#include <etch/parser/module.hpp>
#include <string_view>

namespace etch {
	analysis::module parse(std::string_view sv);
} // namespace etch

#endif
