#include <etch/analysis/semantics.hpp>
#include <etch/codegen.hpp>
#include <etch/compiler.hpp>
#include <etch/parser.hpp>
#include <etch/transform/fold.hpp>

namespace etch {
	std::string etch::compile(std::string_view sv) {
		auto sm = parse(sv);
		auto am = analysis::semantics{}.run(sm);

		transform::fold{}.run(am);

		auto ll = codegen{}.run(am);
		return ll;
	}
} // namespace etch
