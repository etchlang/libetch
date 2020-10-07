#include <etch/parser.hpp>

namespace etch {
	ast::module parse(std::string_view sv) {
		namespace x3 = boost::spirit::x3;

		ast::module m;

		auto it = sv.begin();
		auto end = sv.end();

		auto r = x3::parse(it, end, module(), m);

		if(!r || it != end) {
			throw std::runtime_error("error");
		}

		return m;
	}
} // namespace etch
