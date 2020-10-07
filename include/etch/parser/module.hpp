#ifndef ETCH_EXPR_HPP
#define ETCH_EXPR_HPP 1

#include <boost/spirit/home/x3.hpp>
#include <etch/ast.hpp>

namespace etch::parser {
	namespace x3 = boost::spirit::x3;

	struct module_class;
	using module_type = x3::rule<module_class, ast::module>;
	BOOST_SPIRIT_DECLARE(module_type)
} // namespace etch::parser

namespace etch {
	const parser::module_type & module();
} // namespace etch

#endif
