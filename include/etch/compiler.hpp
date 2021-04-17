#ifndef ETCH_COMPILER_HPP
#define ETCH_COMPILER_HPP 1

#include <etch/analysis/semantics.hpp>
#include <etch/parser.hpp>
#include <string_view>

namespace etch {
	analysis::module compile(std::string_view sv) {
		auto sm = parse(sv);
		return analysis::semantics{}.run(sm);
	}
} // namespace etch

#endif
