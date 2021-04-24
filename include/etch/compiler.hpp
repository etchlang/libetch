#ifndef ETCH_COMPILER_HPP
#define ETCH_COMPILER_HPP 1

#include <llvm/IR/Module.h>
#include <string_view>

namespace etch {
	class compiler {
	  public:
		enum class target {
			llvm_assembly,
			assembly,
			binary
		};
	  private:
		std::shared_ptr<llvm::LLVMContext> ctx = std::make_shared<llvm::LLVMContext>();
		std::shared_ptr<llvm::Module> m;
	  public:
		bool debug = false;
		target tgt = target::binary;

		compiler(std::string name = "a.e") : m(std::make_shared<llvm::Module>(name, *ctx)) {}

		std::string run(std::string_view);
	};
} // namespace etch

#endif
