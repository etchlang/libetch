#ifndef ETCH_AST_ADAPTED_HPP
#define ETCH_AST_ADAPTED_HPP 1

#include <boost/fusion/include/adapt_struct.hpp>
#include <etch/ast.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	etch::ast::integer,
	value
)

BOOST_FUSION_ADAPT_STRUCT(
	etch::ast::definition,
	name, value
)

BOOST_FUSION_ADAPT_STRUCT(
	etch::ast::function,
	args, value
)

BOOST_FUSION_ADAPT_STRUCT(
	etch::ast::call,
	callable, arg
)

BOOST_FUSION_ADAPT_STRUCT(
	etch::ast::op,
	lhs, opname, rhs
)

BOOST_FUSION_ADAPT_STRUCT(
	etch::ast::typed<etch::ast::primary>,
	value, type
)

#endif
