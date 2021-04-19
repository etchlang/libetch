#include <etch/analysis/semantics.hpp>
#include <etch/codegen.hpp>
#include <etch/compiler.hpp>
#include <etch/parser.hpp>
#include <etch/transform/fold.hpp>
#include <llvm/IR/Module.h>

namespace etch {
	std::string etch::compile(std::string_view sv) {
		auto sm = parse(sv);
		auto am = analysis::semantics{}.run(sm);

		transform::fold{}.run(am);

		std::shared_ptr<llvm::LLVMContext> ctx = std::make_shared<llvm::LLVMContext>();
		std::shared_ptr<llvm::Module> m = std::make_shared<llvm::Module>("a.e", *ctx);

		codegen{ctx, m}.run(am);

		llvm::verifyModule(*m, &llvm::errs());

		// output LLVM assembly to string
		std::string ll;
		llvm::raw_string_ostream os(ll);
		os << *m;
		os.flush();

		return ll;
	}
} // namespace etch
