#ifndef ETCH_EXPR_HPP
#define ETCH_EXPR_HPP 1

#include <boost/spirit/home/x3.hpp>
#include <etch/syntax/types.hpp>

namespace etch::parser {
	namespace x3 = boost::spirit::x3;

	struct unit_class;
	using unit_type = x3::rule<unit_class, syntax::unit>;
	BOOST_SPIRIT_DECLARE(unit_type)
} // namespace etch::parser

namespace etch {
	const parser::unit_type & unit();
} // namespace etch

#endif
