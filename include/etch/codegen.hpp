#ifndef ETCH_CODEGEN_HPP
#define ETCH_CODEGEN_HPP 1

#include <etch/analysis/types.hpp>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

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

		llvm::LLVMContext ctx;
		llvm::Module m;

		std::unordered_map<std::string, std::shared_ptr<scope>> scopes;
		std::shared_ptr<scope> scope_module = std::make_shared<scope>();
	  public:
		codegen(std::string name = "a.e") : m(name, ctx) {}

		llvm::Type * type(analysis::value::ptr);

		llvm::Constant * constant(std::shared_ptr<analysis::value::constant_integer>);

		llvm::Value * run(std::shared_ptr<scope>, llvm::IRBuilder<> &, analysis::value::ptr);
		llvm::Constant * run(std::shared_ptr<analysis::value::definition>);
		std::string run(const analysis::module_ &);
	};
} // namespace etch

#endif
