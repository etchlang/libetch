#ifndef ETCH_EXPR_DEF_HPP
#define ETCH_EXPR_DEF_HPP 1

#include <boost/spirit/home/x3.hpp>
#include <etch/ast.hpp>
#include <etch/ast_adapted.hpp>
#include <etch/parser/module.hpp>

namespace etch::parser {
	namespace x3 = boost::spirit::x3;

	// rules

	const module_type module;
	const x3::rule<struct statement_class,  ast::statement>  statement;
	const x3::rule<struct expr_class,       ast::expr>       expr;
	const x3::rule<struct compound_class,   ast::compound>   compound;
	const x3::rule<struct atom_class,       ast::atom>       atom;
	const x3::rule<struct primary_class,    ast::primary>    primary;
	const x3::rule<struct definition_class, ast::definition> definition;
	const x3::rule<struct op_class,         ast::op>         op;
	const x3::rule<struct block_class,      ast::block>      block;
	const x3::rule<struct tuple_class,      ast::tuple>      tuple;
	const x3::rule<struct identifier_class, ast::identifier> identifier;
	const x3::rule<struct integer_class,    ast::integer>    integer;

	// grammar

	const auto char_space  = x3::char_(" \r\n\t\v\f");
	const auto char_opname = x3::char_('+') | x3::char_('-') | x3::char_('*')
	                       | x3::char_('/') | x3::char_('>') | x3::char_('<');

	const auto char_ident_first = x3::char_("A-Za-z");
	const auto char_ident_rest  = x3::char_("A-Za-z0-9");

	const auto ws = x3::omit[*char_space];

	const auto opname = ws >> +char_opname >> ws;

	const auto module_def     = *statement;
	const auto statement_def  = definition | expr;
	const auto expr_def       = compound;
	const auto compound_def   = op | atom;
	const auto atom_def       = primary;
	const auto primary_def    = block | tuple | identifier | integer;
	const auto definition_def = identifier >> '=' >> expr;
	const auto op_def         = atom >> opname >> compound;
	const auto block_def      = ws >> '{' >> *expr >> '}' >> ws;
	const auto tuple_def      = ws >> '(' >> (expr % ',') >> ')' >> ws;
	const auto identifier_def = ws >> char_ident_first >> *char_ident_rest >> ws;
	const auto integer_def    = ws >> x3::int_ >> ws;

	BOOST_SPIRIT_DEFINE(
		module, statement, expr, compound, atom, primary, definition, op, block,
		tuple, identifier, integer
	)
} // namespace etch::parser

namespace etch {
	const parser::module_type & module() {
		return parser::module;
	}
} // namespace etch

#endif
