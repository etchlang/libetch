#include <etch/analysis/semantics.hpp>
#include <etch/codegen.hpp>
#include <etch/compiler.hpp>
#include <etch/parser.hpp>

namespace etch {
	std::string etch::compile(std::string_view sv) {
		auto sm = parse(sv);
		auto m = analysis::semantics{}.run(sm);
		auto ll = codegen{}.run(m);
		return ll;
	}
} // namespace etch
