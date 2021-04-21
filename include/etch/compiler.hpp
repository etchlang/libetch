#ifndef ETCH_COMPILER_HPP
#define ETCH_COMPILER_HPP 1

#include <llvm/IR/Module.h>
#include <string_view>

namespace etch {
	class compiler {
		std::shared_ptr<llvm::LLVMContext> ctx = std::make_shared<llvm::LLVMContext>();
		std::shared_ptr<llvm::Module> m;
	  public:
		bool interpreter = false;

		compiler(std::string name, bool interpreter = false) : m(std::make_shared<llvm::Module>(name, *ctx)), interpreter(interpreter) {}
		compiler(bool interpreter = false) : compiler("a.e", interpreter) {}

		std::string run(std::string_view);
	};
} // namespace etch

#endif
