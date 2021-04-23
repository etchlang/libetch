#include <etch/parser.hpp>
#include <etch/analysis/semantics.hpp>
#include <iostream>

namespace etch {
	syntax::unit parse(std::string_view sv) {
		namespace x3 = boost::spirit::x3;

		syntax::unit m;

		auto it = sv.begin();
		auto end = sv.end();

		auto r = x3::parse(it, end, unit(), m);

		if(!r || it != end) {
			throw std::runtime_error("error");
		}

		return m;
	}
} // namespace etch
