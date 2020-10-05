#include <etch/ast_adapted.hpp>
#include <etch/expr.hpp>
#include <etch/parser.hpp>

namespace etch {
	ast::expr parse(std::string_view sv) {
		namespace x3 = boost::spirit::x3;

		ast::expr ast;

		auto it = sv.begin();
		auto end = sv.end();

		x3::parse(it, end, expr(), ast);
		return ast;
	}
} // namespace etch
