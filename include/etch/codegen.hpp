#ifndef ETCH_CODEGEN_HPP
#define ETCH_CODEGEN_HPP 1

#include <etch/analysis/types.hpp>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

namespace etch {
	class codegen {
		llvm::LLVMContext ctx;
		llvm::Module m;
	  public:
		codegen(std::string name = "a.e") : m(name, ctx) {}

		llvm::Type * type(analysis::value::ptr);

		llvm::Constant * constant(std::shared_ptr<analysis::value::constant_integer>);

		llvm::Value * run(llvm::IRBuilder<> &, analysis::value::ptr);
		void run(std::shared_ptr<analysis::value::definition>);
		std::string run(const analysis::module_ &);
	};
} // namespace etch

#endif
