#ifndef ETCH_COMPILER_HPP
#define ETCH_COMPILER_HPP 1

#include <etch/analysis/semantics.hpp>
#include <etch/codegen.hpp>
#include <etch/parser.hpp>
#include <string_view>

namespace etch {
	analysis::module_ compile(std::string_view sv) {
		auto sm = parse(sv);
		auto m = analysis::semantics{}.run(sm);
		codegen(m);
		return m;
	}
} // namespace etch

#endif
