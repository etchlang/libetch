#ifndef ETCH_TRANSFORM_FOLD_HPP
#define ETCH_TRANSFORM_FOLD_HPP 1

namespace etch::transform {
	class fold {
	  public:
		analysis::value::ptr run(analysis::value::ptr val) {
			if(val == nullptr) { return val; }

			auto r = val;

			if(std::dynamic_pointer_cast<analysis::value::constant_integer>(val)) {
			} else if(std::dynamic_pointer_cast<analysis::value::identifier>(val)) {
			} else if(std::dynamic_pointer_cast<analysis::value::intrinsic>(val)) {
			} else if(auto call = std::dynamic_pointer_cast<analysis::value::call>(val)) {
				call->fn = run(call->fn);
				call->arg = run(call->arg);

				if(auto id = std::dynamic_pointer_cast<analysis::value::identifier>(call->fn)) {
					if(auto t = std::dynamic_pointer_cast<analysis::value::tuple>(call->arg)) {
						if(t->vals.size() == 2) {
							auto lhs = std::dynamic_pointer_cast<analysis::value::constant_integer>(t->vals[0]);
							auto rhs = std::dynamic_pointer_cast<analysis::value::constant_integer>(t->vals[1]);
							if(lhs && rhs) {
								if(id->str == "+") {
									r = std::make_shared<analysis::value::constant_integer>(lhs->val + rhs->val);
								} else if(id->str == "*") {
									r = std::make_shared<analysis::value::constant_integer>(lhs->val * rhs->val);
								}
							}
						}
					}
				} else if(auto intr = std::dynamic_pointer_cast<analysis::value::intrinsic>(call->fn)) {
					if(intr->str == "int") {
						auto c = std::dynamic_pointer_cast<analysis::value::constant_integer>(call->arg);
						if(c) {
							r = std::make_shared<analysis::value::type_int>(c->val);
						}
					}
				}
			} else if(auto def = std::dynamic_pointer_cast<analysis::value::definition>(val)) {
				def->val = run(def->val);
			} else if(auto tuple = std::dynamic_pointer_cast<analysis::value::tuple>(val)) {
				auto result = std::make_shared<analysis::value::tuple>();

				for(auto &val : tuple->vals) {
					auto new_val = run(val);
					auto new_tuple = std::dynamic_pointer_cast<analysis::value::tuple>(new_val);
					if(!new_tuple || new_tuple->vals.size() > 0) {
						result->push_back(new_val);
					}
				}

				if(result->vals.size() == 1) {
					r = result->vals[0];
				} else {
					r = result;
				}
			} else if(auto block = std::dynamic_pointer_cast<analysis::value::block>(val)) {
				for(auto &val : block->vals) {
					val = run(val);
				}

				if(block->vals.size() == 1) {
					r = block->vals[0];
				}
			} else if(auto fn = std::dynamic_pointer_cast<analysis::value::function>(val)) {
				fn->arg = run(fn->arg);
				fn->body = run(fn->body);
			} else if(auto m = std::dynamic_pointer_cast<analysis::value::module_>(val)) {
				for(auto &def : m->defs) {
					def = run(def);
				}
			} else if(auto ty = std::dynamic_pointer_cast<analysis::value::type_type>(val)) {
				// bail to avoid recursing with null type
				return r;
			} else if(std::dynamic_pointer_cast<analysis::value::type_unresolved>(val)) {
			} else if(std::dynamic_pointer_cast<analysis::value::type_int>(val)) {
			} else if(auto ty = std::dynamic_pointer_cast<analysis::value::type_tuple>(val)) {
				auto result = std::make_shared<analysis::value::type_tuple>();

				for(auto &ty_inner : ty->tys) {
					auto new_ty = run(ty_inner);
					auto new_tuple = std::dynamic_pointer_cast<analysis::value::type_tuple>(new_ty);
					if(!new_tuple || new_tuple->tys.size() > 0) {
						result->push_back(new_ty);
					}
				}

				if(result->tys.size() == 1) {
					r = result->tys[0];
				} else {
					r = result;
				}
			} else if(auto ty = std::dynamic_pointer_cast<analysis::value::type_function>(val)) {
				ty->arg = run(ty->arg);
				ty->body = run(ty->body);
			} else if(std::dynamic_pointer_cast<analysis::value::type_module>(val)) {
			} else {
				std::ostringstream s;
				s << "analysis::fold: unhandled value: ";
				val->dump(s);
				auto str = s.str();

				std::cerr << str << std::endl << std::endl;
				throw std::runtime_error(s.str());
			}

			val->ty = run(val->ty);

			return r;
		}

		void run(analysis::unit &au) {
			for(auto &am : au.modules) {
				run(am);
			}
		}
	};
} // namespace etch::transform

#endif
