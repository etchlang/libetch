#include <etch/codegen.hpp>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>

namespace etch {
	llvm::Type * codegen::type(analysis::value::ptr ty) {
		llvm::Type *r = nullptr;

		if(auto ty_int = std::dynamic_pointer_cast<analysis::value::type_int>(ty)) {
			r = llvm::Type::getIntNTy(ctx, ty_int->width);
		}

		return r;
	}

	llvm::Constant * codegen::constant(std::shared_ptr<analysis::value::constant_integer> val) {
		llvm::Constant *r = nullptr;

		auto lty = type(val->ty);
		if(auto ty_int = std::dynamic_pointer_cast<analysis::value::type_int>(val->ty)) {
			llvm::APInt ap(ty_int->width, val->val);
			r = llvm::Constant::getIntegerValue(lty, ap);
		}

		return r;
	}

	void codegen::run(std::shared_ptr<analysis::value::definition> def) {
		if(auto i = std::dynamic_pointer_cast<analysis::value::constant_integer>(def->val)) {
			auto ty = type(def->ty);

			auto c = constant(i);
			auto gv = new llvm::GlobalVariable(m, c->getType(), true, llvm::Function::ExternalLinkage, c, def->name.str);
		}
	}

	std::string codegen::run(const analysis::module_ &am) {
		for(auto &val : am.defs) {
			if(auto def = std::dynamic_pointer_cast<analysis::value::definition>(val)) {
				run(def);
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
