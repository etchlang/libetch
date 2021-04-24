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
				for(auto &arg : call->args) {
					arg = run(arg);
				}

				if(auto id = std::dynamic_pointer_cast<analysis::value::identifier>(call->fn)) {
					if(call->args.size() == 2) {
						auto lhs = std::dynamic_pointer_cast<analysis::value::constant_integer>(call->args[0]);
						auto rhs = std::dynamic_pointer_cast<analysis::value::constant_integer>(call->args[1]);
						if(lhs && rhs) {
							if(id->str == "+") {
								r = std::make_shared<analysis::value::constant_integer>(lhs->val + rhs->val);
							} else if(id->str == "*") {
								r = std::make_shared<analysis::value::constant_integer>(lhs->val * rhs->val);
							}
						}
					}
				} else if(auto intr = std::dynamic_pointer_cast<analysis::value::intrinsic>(call->fn)) {
					if(intr->str == "int") {
						auto c = std::dynamic_pointer_cast<analysis::value::constant_integer>(call->args[0]);
						if(c) {
							r = std::make_shared<analysis::value::type_int>(c->val);
						}
					}
				}
			} else if(auto def = std::dynamic_pointer_cast<analysis::value::definition>(val)) {
				def->val = run(def->val);
			} else if(auto tuple = std::dynamic_pointer_cast<analysis::value::tuple>(val)) {
				for(auto &val : tuple->vals) {
					val = run(val);
				}

				if(tuple->vals.size() == 1) {
					r = tuple->vals[0];
				}
			} else if(auto block = std::dynamic_pointer_cast<analysis::value::block>(val)) {
				for(auto &val : block->vals) {
					val = run(val);
				}

				if(block->vals.size() == 1) {
					r = block->vals[0];
				}
			} else if(auto fn = std::dynamic_pointer_cast<analysis::value::function>(val)) {
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
				for(auto &ty_inner : ty->tys) {
					ty_inner = run(ty_inner);
				}

				if(ty->tys.size() == 1) {
					r = ty->tys[0];
				}
			} else if(auto ty = std::dynamic_pointer_cast<analysis::value::type_function>(val)) {
				for(auto &arg : ty->args) {
					arg = run(arg);
				}
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
