#ifndef ETCH_SYNTAX_ADAPTED_HPP
#define ETCH_SYNTAX_ADAPTED_HPP 1

#include <boost/fusion/include/adapt_struct.hpp>
#include <etch/syntax/types.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	etch::syntax::integer,
	value
)

BOOST_FUSION_ADAPT_STRUCT(
	etch::syntax::definition,
	name, value
)

BOOST_FUSION_ADAPT_STRUCT(
	etch::syntax::function,
	args, value
)

BOOST_FUSION_ADAPT_STRUCT(
	etch::syntax::call,
	callable, arg
)

BOOST_FUSION_ADAPT_STRUCT(
	etch::syntax::op,
	lhs, opname, rhs
)

BOOST_FUSION_ADAPT_STRUCT(
	etch::syntax::typed<etch::syntax::primary>,
	value, type
)

#endif
