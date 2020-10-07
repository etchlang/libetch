#include <etch/parser.hpp>

namespace etch {
	ast::expr parse(std::string_view sv) {
		namespace x3 = boost::spirit::x3;

		ast::expr ast;

		auto it = sv.begin();
		auto end = sv.end();

		auto r = x3::parse(it, end, expr(), ast);

		if(!r || it != end) {
			throw std::runtime_error("error");
		}

		return ast;
	}
} // namespace etch
