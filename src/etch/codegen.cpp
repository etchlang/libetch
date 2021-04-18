#include <etch/codegen.hpp>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>

namespace etch {
	llvm::Type * codegen::type(analysis::value::ptr ty) {
		llvm::Type *r = nullptr;

		if(auto ty_int = std::dynamic_pointer_cast<analysis::value::type_int>(ty)) {
			r = llvm::Type::getIntNTy(ctx, ty_int->width);
		} else if(auto ty_fn = std::dynamic_pointer_cast<analysis::value::type_function>(ty)) {
			auto lty_ret = type(ty_fn->body);
			r = llvm::FunctionType::get(lty_ret, false);
		} else if(auto ty_tuple = std::dynamic_pointer_cast<analysis::value::type_tuple>(ty)) {
			if(ty_tuple->tys.empty()) {
				r = llvm::Type::getVoidTy(ctx);
			}
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

	llvm::Value * codegen::run(llvm::IRBuilder<> &builder, analysis::value::ptr val) {
		llvm::Value *r = nullptr;

		if(auto i = std::dynamic_pointer_cast<analysis::value::constant_integer>(val)) {
			r = constant(i);
		}
		return r;
	}

	void codegen::run(std::shared_ptr<analysis::value::definition> def) {
		if(auto i = std::dynamic_pointer_cast<analysis::value::constant_integer>(def->val)) {
			auto c = constant(i);
			auto gv = new llvm::GlobalVariable(m, c->getType(), true, llvm::Function::ExternalLinkage, c, def->name.str);
		} else if(auto fn = std::dynamic_pointer_cast<analysis::value::function>(def->val)) {
			auto lty_fn = llvm::cast<llvm::FunctionType>(type(def->val->ty));
			auto f = llvm::Function::Create(lty_fn, llvm::Function::ExternalLinkage, def->name.str, m);

			auto bb = llvm::BasicBlock::Create(ctx, "entry", f);

			llvm::IRBuilder<> builder(bb);
			if(auto r = run(builder, fn->body)) {
				builder.CreateRet(r);
			} else {
				builder.CreateRetVoid();
			}
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
