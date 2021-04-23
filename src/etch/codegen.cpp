#include <etch/codegen.hpp>
#include <etch/mangling.hpp>

namespace etch {
	llvm::Type * codegen::type(analysis::value::ptr ty) {
		llvm::Type *r = nullptr;

		if(auto ty_int = std::dynamic_pointer_cast<analysis::value::type_int>(ty)) {
			r = llvm::Type::getIntNTy(*ctx, (unsigned int)ty_int->width);
		} else if(auto ty_tuple = std::dynamic_pointer_cast<analysis::value::type_tuple>(ty)) {
			if(ty_tuple->tys.empty()) {
				r = llvm::Type::getVoidTy(*ctx);
			}
		} else if(auto ty_fn = std::dynamic_pointer_cast<analysis::value::type_function>(ty)) {
			std::vector<llvm::Type *> lty_args;
			for(auto &arg : ty_fn->args) {
				lty_args.push_back(type(arg));
			}

			auto lty_ret = type(ty_fn->body);

			r = llvm::FunctionType::get(lty_ret, lty_args, false);
		}

		return r;
	}

	llvm::Constant * codegen::constant(std::shared_ptr<analysis::value::constant_integer> val) {
		llvm::Constant *r = nullptr;

		auto lty = type(val->ty);
		if(auto ty_int = std::dynamic_pointer_cast<analysis::value::type_int>(val->ty)) {
			llvm::APInt ap((unsigned int)ty_int->width, val->val);
			r = llvm::Constant::getIntegerValue(lty, ap);
		}

		return r;
	}

	llvm::Value * codegen::run(std::shared_ptr<scope> scp, llvm::IRBuilder<> &builder, analysis::value::ptr val) {
		llvm::Value *r = nullptr;

		if(auto i = std::dynamic_pointer_cast<analysis::value::constant_integer>(val)) {
			r = constant(i);
		} else if(auto id = std::dynamic_pointer_cast<analysis::value::identifier>(val)) {
			auto sym = scp->find(id->str);
			if(llvm::isa<llvm::GlobalVariable>(sym) || llvm::isa<llvm::GlobalAlias>(sym)) {
				auto lty = sym->getType()->getPointerElementType();
				r = builder.CreateLoad(lty, sym);
			} else {
				r = sym;
			}
		} else if(auto call = std::dynamic_pointer_cast<analysis::value::call>(val)) {
			auto id = std::dynamic_pointer_cast<analysis::value::identifier>(call->fn);
			if(id && id->str == "+") {
				auto lhs = run(scp, builder, call->args[0]);
				auto rhs = run(scp, builder, call->args[1]);
				r = builder.CreateAdd(lhs, rhs);
			} else {
				auto f = llvm::cast<llvm::Function>(run(scp, builder, call->fn));

				std::vector<llvm::Value *> args;
				for(auto &arg : call->args) {
					if(auto v = run(scp, builder, arg)) {
						args.emplace_back(v);
					}
				}
				r = builder.CreateCall(f->getFunctionType(), f, args);
			}
		} else if(auto def = std::dynamic_pointer_cast<analysis::value::definition>(val)) {
			auto val = run(scp, builder, def->val);
			val->setName(def->name.str);

			scp->push(def->name.str, val);

			r = val;
		} else if(auto tuple = std::dynamic_pointer_cast<analysis::value::tuple>(val)) {
		} else if(auto block = std::dynamic_pointer_cast<analysis::value::block>(val)) {
			for(auto &val : block->vals) {
				r = run(scp, builder, val);
			}
		} else {
			std::cout << "UNHANDLED VALUE: ";
			val->dump() << std::endl;
		}

		return r;
	}

	llvm::Constant * codegen::run(std::shared_ptr<analysis::value::definition> def) {
		llvm::Constant *r = nullptr;

		stack.emplace_back(def->name.str);
		auto mangled = mangle(stack);

		if(auto i = std::dynamic_pointer_cast<analysis::value::constant_integer>(def->val)) {
			auto c = constant(i);
			r = new llvm::GlobalVariable(*m, c->getType(), true, llvm::GlobalValue::ExternalLinkage, c, mangled);
		} else if(auto id = std::dynamic_pointer_cast<analysis::value::identifier>(def->val)) {
			auto gv = llvm::cast<llvm::GlobalValue>(scope_module->find(id->str));
			r = llvm::GlobalAlias::create(mangled, gv);
		} else if(auto fn = std::dynamic_pointer_cast<analysis::value::function>(def->val)) {
			auto scp = std::make_shared<scope>(scope_module);

			auto lty_fn = llvm::cast<llvm::FunctionType>(type(def->val->ty));
			auto f = llvm::Function::Create(lty_fn, llvm::Function::ExternalLinkage, mangled, *m);

			for(size_t i = 0; i < fn->args.size(); ++i) {
				auto name = std::dynamic_pointer_cast<analysis::value::identifier>(fn->args[i]);

				auto arg = f->getArg((unsigned int)i);
				arg->setName(name->str);

				scp->push(name->str, arg);
			}

			auto bb = llvm::BasicBlock::Create(*ctx, "entry", f);

			llvm::IRBuilder<> builder(bb);
			if(auto ret = run(scp, builder, fn->body)) {
				builder.CreateRet(ret);
			} else {
				builder.CreateRetVoid();
			}

			r = f;
		} else if(auto m = std::dynamic_pointer_cast<analysis::value::module_>(def->val)) {
			run(m);
		} else {
			std::cout << "UNHANDLED GLOBAL VALUE: ";
			def->val->dump() << std::endl;
		}

		stack.pop_back();

		scope_module->push(def->name.str, r);
		return r;
	}

	void codegen::run(std::shared_ptr<analysis::value::module_> am) {
		for(auto &val : am->defs) {
			if(auto def = std::dynamic_pointer_cast<analysis::value::definition>(val)) {
				run(def);
			}
		}
	}

	void codegen::run(const analysis::unit &au) {
		for(auto &am : au.modules) {
			run(am);
		}
	}
} // namespace etch
