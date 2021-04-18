#include <etch/codegen.hpp>
#include <etch/analysis/semantics.hpp>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>

namespace etch {
	std::string codegen(const analysis::module_ &am) {
		llvm::LLVMContext ctx;
		llvm::Module m("module.e", ctx);

		for(auto &val : am.defs) {
			if(auto def = std::dynamic_pointer_cast<analysis::value::definition>(val)) {
				if(auto i = std::dynamic_pointer_cast<analysis::value::constant_integer>(def->val)) {
					auto ty_i32 = llvm::Type::getIntNTy(ctx, 32);

					auto gv = new llvm::GlobalVariable(m, ty_i32, true, llvm::Function::ExternalLinkage, nullptr, def->name.str);

					llvm::APInt ap(32, i->val);
					auto constant = llvm::Constant::getIntegerValue(ty_i32, ap);
					gv->setInitializer(constant);
				}
			}
		}

		llvm::verifyModule(m, &llvm::errs());

		// output LLVM assembly to string
		std::string str;
		llvm::raw_string_ostream os(str);
		os << m;
		os.flush();

		return str;
	}
} // namespace etch
