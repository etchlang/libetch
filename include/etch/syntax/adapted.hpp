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
	binding, value
)

BOOST_FUSION_ADAPT_STRUCT(
	etch::syntax::function,
	arg, body
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
