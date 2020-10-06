#ifndef ETCH_EXPR_HPP
#define ETCH_EXPR_HPP 1

#include <boost/spirit/home/x3.hpp>
#include <etch/ast.hpp>

namespace etch::parser {
	namespace x3 = boost::spirit::x3;

	struct expr_class;
	using expr_type = x3::rule<expr_class, ast::expr>;
	BOOST_SPIRIT_DECLARE(expr_type)
} // namespace etch::parser

namespace etch {
	const parser::expr_type & expr();
} // namespace etch

#endif
