#ifndef ETCH_COMPILER_HPP
#define ETCH_COMPILER_HPP 1

#include <etch/analysis/semantics.hpp>
#include <etch/codegen.hpp>
#include <etch/parser.hpp>
#include <string_view>

namespace etch {
	std::string compile(std::string_view sv) {
		auto sm = parse(sv);
		auto m = analysis::semantics{}.run(sm);
		auto ll = codegen(m);
		return ll;
	}
} // namespace etch

#endif
