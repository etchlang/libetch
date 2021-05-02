#ifndef ETCH_CODEGEN_HPP
#define ETCH_CODEGEN_HPP 1

#include <etch/ir/types.hpp>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>

namespace etch {
	class codegen {
		class scope {
			std::shared_ptr<scope> parent;
			std::unordered_map<std::string, llvm::Value *> syms;
		  public:
			scope() = default;
			scope(std::shared_ptr<scope> parent) : parent(parent) {}

			void push(std::string name, llvm::Value *val) {
				syms[name] = val;
			}

			llvm::Value * find(std::string name) const {
				auto it = syms.find(name);
				if(it != syms.end()) {
					return it->second;
				} else if(parent) {
					return parent->find(name);
				} else {
					return nullptr;
				}
			}
		};

		std::shared_ptr<llvm::LLVMContext> ctx;
		std::shared_ptr<llvm::Module> m;

		std::shared_ptr<scope> scope_module = std::make_shared<scope>();

		std::vector<std::string> stack;
	  public:
		codegen(std::shared_ptr<llvm::LLVMContext> ctx, std::shared_ptr<llvm::Module> m) : ctx(ctx), m(m) {}

		void bind(std::shared_ptr<scope>, llvm::IRBuilder<> &, ir::ptr, llvm::Value *);

		llvm::Type     * type(ir::ptr);
		llvm::Constant * constant(ir::ptr);
		llvm::Function * function(std::string, std::shared_ptr<ir::function>);
		llvm::Value    * local(std::shared_ptr<scope>, llvm::IRBuilder<> &, ir::ptr);
		llvm::Constant * global(ir::ptr);

		void run(std::shared_ptr<ir::module_>);
		void run(const ir::unit &);
	};
} // namespace etch

#endif
