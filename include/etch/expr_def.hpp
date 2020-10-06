#ifndef ETCH_EXPR_DEF_HPP
#define ETCH_EXPR_DEF_HPP 1

#include <boost/spirit/home/x3.hpp>
#include <etch/ast.hpp>
#include <etch/ast_adapted.hpp>
#include <etch/expr.hpp>

namespace etch::parser {
	namespace x3 = boost::spirit::x3;

	// rule IDs

	struct compound_class;
	struct atom_class;
	struct primary_class;
	struct op_class;

	// rules

	const expr_type expr                                   = "expr";
	const x3::rule<compound_class, ast::compound> compound = "compound";
	const x3::rule<atom_class,     ast::atom>     atom     = "atom";
	const x3::rule<primary_class,  ast::primary>  primary  = "primary";
	const x3::rule<op_class,       ast::op>       op       = "op";

	// grammar

	const auto char_space  = x3::char_(" \r\n\t\v\f");
	const auto char_opname = x3::char_('+')
	                       | x3::char_('-')
	                       | x3::char_('*')
	                       | x3::char_('/');

	const auto ws = x3::omit[*char_space];

	const auto number = ws >> x3::int_ >> ws;
	const auto opname = ws >> *char_opname >> ws;

	const auto expr_def     = compound;
	const auto compound_def = op | atom;
	const auto atom_def     = primary;
	const auto primary_def  = number;
	const auto op_def       = atom >> opname >> compound;

	BOOST_SPIRIT_DEFINE(expr, compound, atom, primary, op)
} // namespace etch::parser

namespace etch {
	const parser::expr_type & expr() {
		return parser::expr;
	}
} // namespace etch

#endif
