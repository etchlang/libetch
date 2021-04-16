#include <etch/parser.hpp>
#include <etch/analysis/semantics.hpp>

#include <iostream>
#include <etch/syntax/dump.hpp>

namespace etch {
	analysis::module parse(std::string_view sv) {
		namespace x3 = boost::spirit::x3;

		syntax::module sm;

		auto it = sv.begin();
		auto end = sv.end();

		auto r = x3::parse(it, end, module(), sm);

		if(!r || it != end) {
			throw std::runtime_error("error");
		}

		auto m = analysis::analysis(sm);
		return m;
	}
} // namespace etch
