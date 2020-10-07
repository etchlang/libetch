#ifndef ETCH_AST_ADAPTED_HPP
#define ETCH_AST_ADAPTED_HPP 1

#include <boost/fusion/include/adapt_struct.hpp>
#include <etch/ast.hpp>

BOOST_FUSION_ADAPT_STRUCT(
	etch::ast::tuple,
	exprs
)

BOOST_FUSION_ADAPT_STRUCT(
	etch::ast::block,
	exprs
)

BOOST_FUSION_ADAPT_STRUCT(
	etch::ast::op,
	lhs, opname, rhs
)

#endif
