#ifndef ETCH_EXPR_DEF_HPP
#define ETCH_EXPR_DEF_HPP 1

#include <boost/spirit/home/x3.hpp>
#include <etch/syntax/types.hpp>
#include <etch/syntax/adapted.hpp>
#include <etch/parser/unit.hpp>

namespace etch::parser {
	namespace x3 = boost::spirit::x3;

	// rules

	const unit_type unit;
	const x3::rule<struct module_class,     syntax::module>     module;
	const x3::rule<struct statement_class,  syntax::statement>  statement;
	const x3::rule<struct expr_class,       syntax::expr>       expr;
	const x3::rule<struct compound_class,   syntax::compound>   compound;
	const x3::rule<struct atom_class,       syntax::atom>       atom;
	const x3::rule<struct primary_class,    syntax::primary>    primary;
	const x3::rule<struct definition_class, syntax::definition> definition;
	const x3::rule<struct function_class,   syntax::function>   function;
	const x3::rule<struct arglist_class,    syntax::arglist>    arglist;
	const x3::rule<struct op_class,         syntax::op>         op;
	const x3::rule<struct block_class,      syntax::block>      block;
	const x3::rule<struct tuple_class,      syntax::tuple>      tuple;
	const x3::rule<struct identifier_class, syntax::identifier> identifier;
	const x3::rule<struct integer_class,    syntax::integer>    integer;

	// grammar

	const auto char_space  = x3::char_(" \r\n\t\v\f");
	const auto char_opname = x3::char_("+*/><=.") | x3::char_('-');

	const auto char_ident_first = x3::char_("A-Za-z_");
	const auto char_ident_rest  = x3::char_("A-Za-z_0-9");

	const auto ws = x3::omit[*char_space];

	const auto opname = ws >> +char_opname >> ws;

	const auto module_expr = ws >> "#{" >> module >> '}' >> ws;

	const auto arg = atom;

	const auto unit_def       = x3::repeat(1)[module];
	const auto module_def     = *statement;
	const auto statement_def  = definition | expr;
	const auto expr_def       = module_expr | function | compound;
	const auto compound_def   = op | atom;
	const auto atom_def       = primary >> ':' >> atom | primary;
	const auto primary_def    = block | tuple | identifier | integer;
	const auto definition_def = identifier >> '=' >> expr;
	const auto function_def   = arglist >> "->" >> expr;
	const auto arglist_def    = ws >> '(' >> ws >> -(arg % ',') >> ws >> ')' >> ws
	                          | x3::repeat(1)[arg];
	const auto op_def         = atom >> opname >> expr;
	const auto block_def      = ws >> '{' >> *statement >> '}' >> ws;
	const auto tuple_def      = ws >> '(' >> -(expr % ',') >> ')' >> ws;
	const auto identifier_def = ws >> char_ident_first >> *char_ident_rest >> ws;
	const auto integer_def    = ws >> x3::int_ >> ws;

	BOOST_SPIRIT_DEFINE(
		unit, module, statement, expr, compound, atom, primary, definition, function,
		arglist, op, block, tuple, identifier, integer
	)
} // namespace etch::parser

namespace etch {
	const parser::unit_type & unit() {
		return parser::unit;
	}
} // namespace etch

#endif
