#ifndef ETCH_TRANSFORM_FOLD_HPP
#define ETCH_TRANSFORM_FOLD_HPP 1

#include <etch/transform/base.hpp>

namespace etch::transform {
	class fold {
	  public:
		ir::ptr run(ir::ptr val) {
			if(val == nullptr) { return val; }

			auto r = val;

			if(std::dynamic_pointer_cast<ir::constant_int>(val)) {
			} else if(std::dynamic_pointer_cast<ir::identifier>(val)) {
			} else if(auto call = std::dynamic_pointer_cast<ir::call>(val)) {
				call->fn = run(call->fn);
				call->arg = run(call->arg);

				auto t = std::dynamic_pointer_cast<ir::tuple>(call->arg);

				if(auto intr = std::dynamic_pointer_cast<ir::intr_int>(call->fn)) {
					auto c = std::dynamic_pointer_cast<ir::constant_int>(call->arg);
					if(c) {
						r = std::make_shared<ir::type_int>(c->val);
					}
				} else if(auto intr = std::dynamic_pointer_cast<ir::intr_add>(call->fn)) {
					auto lhs = std::dynamic_pointer_cast<ir::constant_int>(t->vals[0]);
					auto rhs = std::dynamic_pointer_cast<ir::constant_int>(t->vals[1]);
					if(lhs && rhs) {
						r = std::make_shared<ir::constant_int>(lhs->val + rhs->val);
					}
				} else if(auto intr = std::dynamic_pointer_cast<ir::intr_mul>(call->fn)) {
					auto lhs = std::dynamic_pointer_cast<ir::constant_int>(t->vals[0]);
					auto rhs = std::dynamic_pointer_cast<ir::constant_int>(t->vals[1]);
					if(lhs && rhs) {
						r = std::make_shared<ir::constant_int>(lhs->val * rhs->val);
					}
				}
			} else if(auto def = std::dynamic_pointer_cast<ir::definition>(val)) {
				def->val = run(def->val);
			} else if(auto tuple = std::dynamic_pointer_cast<ir::tuple>(val)) {
				auto result = std::make_shared<ir::tuple>();

				for(auto &val : tuple->vals) {
					auto new_val = run(val);
					auto new_tuple = std::dynamic_pointer_cast<ir::tuple>(new_val);
					if(!new_tuple || new_tuple->vals.size() > 0) {
						result->push_back(new_val);
					}
				}

				if(result->vals.size() == 1) {
					r = result->vals[0];
				} else {
					r = result;
				}
			} else if(auto block = std::dynamic_pointer_cast<ir::block>(val)) {
				for(auto &val : block->vals) {
					val = run(val);
				}

				if(block->vals.size() == 1) {
					r = block->vals[0];
				}
			} else if(auto fn = std::dynamic_pointer_cast<ir::function>(val)) {
				fn->arg = run(fn->arg);
				fn->body = run(fn->body);
			} else if(auto m = std::dynamic_pointer_cast<ir::module_>(val)) {
				for(auto &def : m->defs) {
					def = run(def);
				}
			} else if(std::dynamic_pointer_cast<ir::intr_int>(val)) {
			} else if(std::dynamic_pointer_cast<ir::intr_add>(val)) {
			} else if(std::dynamic_pointer_cast<ir::intr_mul>(val)) {
			} else if(auto cast = std::dynamic_pointer_cast<ir::cast>(val)) {
				cast->ty  = run(cast->ty);
				cast->val = run(cast->val);

				if(auto ty_int = std::dynamic_pointer_cast<ir::type_int>(cast->ty)) {
					if(auto val_int = std::dynamic_pointer_cast<ir::constant_int>(cast->val)) {
						r = std::make_shared<ir::constant_int>(val_int->val, ty_int->width);
					}
				}
			} else if(auto ty = std::dynamic_pointer_cast<ir::type_type>(val)) {
				// bail to avoid infinite loop
				return r;
			} else if(std::dynamic_pointer_cast<ir::type_unresolved>(val)) {
			} else if(std::dynamic_pointer_cast<ir::type_int>(val)) {
			} else {
				std::ostringstream s;
				s << "analysis::fold: unhandled value: ";
				val->dump(s);
				auto str = s.str();

				std::cerr << str << std::endl << std::endl;
				throw std::runtime_error(s.str());
			}

			return r;
		}

		void run(ir::unit &au) {
			for(auto &am : au.modules) {
				run(am);
			}
		}
	};
} // namespace etch::transform

#endif
