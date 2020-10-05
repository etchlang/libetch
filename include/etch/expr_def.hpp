#ifndef ETCH_EXPR_DEF_HPP
#define ETCH_EXPR_DEF_HPP 1

#include <boost/spirit/home/x3.hpp>
#include <etch/ast.hpp>
#include <etch/ast_adapted.hpp>
#include <etch/expr.hpp>

namespace etch::parser {
	namespace x3 = boost::spirit::x3;

	const expr_type expr = "expr";

	const auto ws = x3::omit[*x3::space];
	const auto expr_def = x3::char_ >> ws >> x3::int_ >> ws >> x3::int_;

	BOOST_SPIRIT_DEFINE(expr)
} // namespace etch::parser

namespace etch {
	parser::expr_type expr() {
		return parser::expr;
	}
} // namespace etch

#endif
