#include <etch/codegen.hpp>
#include <etch/mangling.hpp>
#include <sstream>

namespace etch {
	llvm::Type * codegen::type(analysis::value::ptr ty) {
		llvm::Type *r = nullptr;

		if(auto ty_int = std::dynamic_pointer_cast<analysis::value::type_int>(ty)) {
			r = llvm::Type::getIntNTy(*ctx, (unsigned int)ty_int->width);
		} else if(auto ty_tuple = std::dynamic_pointer_cast<analysis::value::type_tuple>(ty)) {
			if(ty_tuple->tys.empty()) {
				r = llvm::Type::getVoidTy(*ctx);
			} else {
				std::vector<llvm::Type *> lty_vals;
				for(auto &ty : ty_tuple->tys) {
					lty_vals.emplace_back(type(ty));
				}

				r = llvm::StructType::get(*ctx, lty_vals);
			}
		} else if(auto ty_fn = std::dynamic_pointer_cast<analysis::value::type_function>(ty)) {
			std::vector<llvm::Type *> lty_args;

			auto lty_arg = type(ty_fn->arg);
			if(!lty_arg->isVoidTy()) {
				lty_args.emplace_back(lty_arg);
			}

			auto lty_ret = type(ty_fn->body);

			if(llvm::isa<llvm::FunctionType>(lty_ret)) {
				lty_ret = lty_ret->getPointerTo();
			}

			r = llvm::FunctionType::get(lty_ret, lty_args, false);
		} else {
			std::ostringstream s;
			s << "codegen: unhandled type: ";
			ty->dump(s);
			auto str = s.str();

			std::cerr << str << std::endl << std::endl;
			throw std::runtime_error(s.str());
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

	void codegen::bind(std::shared_ptr<scope> scp, llvm::IRBuilder<> &builder, analysis::value::ptr val, llvm::Value *lval) {
		if(auto id = std::dynamic_pointer_cast<analysis::value::identifier>(val)) {
			scp->push(id->str, lval);
		} else if(auto tuple = std::dynamic_pointer_cast<analysis::value::tuple>(val)) {
			for(size_t i = 0; i < tuple->vals.size(); ++i) {
				std::array<unsigned, 1> indices = {(unsigned)i};
				auto el = builder.CreateExtractValue(lval, indices);
				bind(scp, builder, tuple->vals[i], el);
			}
		} else {
			std::ostringstream s;
			s << "codegen: unhandled binding: ";
			val->dump(s);
			auto str = s.str();

			std::cerr << str << std::endl << std::endl;
			throw std::runtime_error(s.str());
		}
	}

	llvm::Function * codegen::function(std::shared_ptr<analysis::value::function> fn, std::string name) {
		auto scp = std::make_shared<scope>(scope_module);

		auto lty_fn = llvm::cast<llvm::FunctionType>(type(fn->ty));
		auto f = llvm::Function::Create(lty_fn, llvm::Function::ExternalLinkage, name, *m);

		auto bb_entry = llvm::BasicBlock::Create(*ctx, "entry", f);
		llvm::IRBuilder<> builder_entry(bb_entry);

		if(lty_fn->getNumParams() == 1) {
			bind(scp, builder_entry, fn->arg, f->getArg(0));
		}

		if(auto ret = run(scp, builder_entry, fn->body)) {
			builder_entry.CreateRet(ret);
		} else {
			builder_entry.CreateRetVoid();
		}

		return f;
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
				auto tuple = std::dynamic_pointer_cast<analysis::value::tuple>(call->arg);
				auto lhs = run(scp, builder, tuple->vals[0]);
				auto rhs = run(scp, builder, tuple->vals[1]);
				r = builder.CreateAdd(lhs, rhs);
			} else if(id && id->str == "*") {
				auto tuple = std::dynamic_pointer_cast<analysis::value::tuple>(call->arg);
				auto lhs = run(scp, builder, tuple->vals[0]);
				auto rhs = run(scp, builder, tuple->vals[1]);
				r = builder.CreateMul(lhs, rhs);
			} else {
				auto fval = run(scp, builder, call->fn);
				auto fvalty = fval->getType();

				if(fvalty->isPointerTy()) {
					fvalty = fvalty->getPointerElementType();
				}

				std::vector<llvm::Value *> args;
				if(auto v = run(scp, builder, call->arg)) {
					args.emplace_back(v);
				}

				auto fty = llvm::cast<llvm::FunctionType>(fvalty);
				auto c = builder.CreateCall(fty, fval, args);
				if(!fty->getReturnType()->isVoidTy()) {
					r = c;
				}
			}
		} else if(auto def = std::dynamic_pointer_cast<analysis::value::definition>(val)) {
			auto val = run(scp, builder, def->val);
			bind(scp, builder, def->binding, val);

			r = val;
		} else if(auto tuple = std::dynamic_pointer_cast<analysis::value::tuple>(val)) {
			auto lty = type(tuple->ty);
			if(!lty->isVoidTy()) {
				llvm::Value *result = llvm::PoisonValue::get(lty);

				for(size_t i = 0; i < tuple->vals.size(); ++i) {
					auto el = run(scp, builder, tuple->vals[i]);
					std::array<unsigned, 1> indices = {(unsigned)i};
					result = builder.CreateInsertValue(result, el, indices);
				}

				r = result;
			}
		} else if(auto block = std::dynamic_pointer_cast<analysis::value::block>(val)) {
			for(auto &val : block->vals) {
				r = run(scp, builder, val);
			}
		} else if(auto fn = std::dynamic_pointer_cast<analysis::value::function>(val)) {
			stack.emplace_back("anon");
			auto mangled = mangle(stack);
			stack.pop_back();

			r = function(fn, mangled);

		} else {
			std::ostringstream s;
			s << "codegen: unhandled value: ";
			val->dump(s);
			auto str = s.str();

			std::cerr << str << std::endl << std::endl;
			throw std::runtime_error(s.str());
		}

		return r;
	}

	llvm::Constant * codegen::run(std::shared_ptr<analysis::value::definition> def) {
		llvm::Constant *r = nullptr;

		auto mangled = mangle(stack);

		if(auto i = std::dynamic_pointer_cast<analysis::value::constant_integer>(def->val)) {
			auto c = constant(i);
			r = new llvm::GlobalVariable(*m, c->getType(), true, llvm::GlobalValue::ExternalLinkage, c, mangled);
		} else if(auto id = std::dynamic_pointer_cast<analysis::value::identifier>(def->val)) {
			auto gv = llvm::cast<llvm::GlobalValue>(scope_module->find(id->str));
			r = llvm::GlobalAlias::create(mangled, gv);
		} else if(auto intr = std::dynamic_pointer_cast<analysis::value::intrinsic>(def->val)) {
			if(intr->str == "int") {
			} else {
				std::ostringstream s;
				s << "codegen: unhandled global intrinsic: ";
				def->val->dump(s);
				auto str = s.str();

				std::cerr << str << std::endl << std::endl;
				throw std::runtime_error(s.str());
			}
		} else if(auto fn = std::dynamic_pointer_cast<analysis::value::function>(def->val)) {
			r = function(fn, mangled);
		} else if(auto m = std::dynamic_pointer_cast<analysis::value::module_>(def->val)) {
			run(m);
		} else if(std::dynamic_pointer_cast<analysis::value::type_int>(def->val)) {
		} else {
			std::ostringstream s;
			s << "codegen: unhandled global: ";
			def->val->dump(s);
			auto str = s.str();

			std::cerr << str << std::endl << std::endl;
			throw std::runtime_error(s.str());
		}

		return r;
	}

	void codegen::run(std::shared_ptr<analysis::value::module_> am) {
		for(auto &val : am->defs) {
			if(auto def = std::dynamic_pointer_cast<analysis::value::definition>(val)) {
				auto id = std::dynamic_pointer_cast<analysis::value::identifier>(def->binding);
				std::string scope_name = id ? id->str : "anon";

				stack.emplace_back(scope_name);

				auto r = run(def);

				stack.pop_back();
				scope_module->push(scope_name, r);
			}
		}
	}

	void codegen::run(const analysis::unit &au) {
		for(auto &am : au.modules) {
			run(am);
		}
	}
} // namespace etch
