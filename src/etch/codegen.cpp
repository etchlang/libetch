#include <etch/codegen.hpp>
#include <etch/parser.hpp>
#include <etch/analysis/semantics.hpp>
#include <iostream>


namespace etch {
	void codegen(const analysis::module_ &am) {
		llvm::LLVMContext ctx;
		llvm::Module m("", ctx);
	}
} // namespace etch
