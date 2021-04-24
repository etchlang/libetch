#ifndef ETCH_TRANSFORM_FOLD_HPP
#define ETCH_TRANSFORM_FOLD_HPP 1

namespace etch::transform {
	class fold {
	  public:
		analysis::value::ptr run(analysis::value::ptr val) {
			if(val == nullptr) { return val; }

			auto r = val;

			if(auto ty_tuple = std::dynamic_pointer_cast<analysis::value::type_tuple>(val)) {
				for(auto &ty : ty_tuple->tys) {
					ty = run(ty);
				}

				if(ty_tuple->tys.size() == 1) {
					r = ty_tuple->tys[0];
				}
			} else if(auto ty_fn = std::dynamic_pointer_cast<analysis::value::type_function>(val)) {
				ty_fn->body = run(ty_fn->body);
			} else if(auto call = std::dynamic_pointer_cast<analysis::value::call>(val)) {
				call->fn = run(call->fn);
				for(auto &arg : call->args) {
					arg = run(arg);
				}

				if(call->args.size() == 2) {
					auto lhs = std::dynamic_pointer_cast<analysis::value::constant_integer>(call->args[0]);
					auto rhs = std::dynamic_pointer_cast<analysis::value::constant_integer>(call->args[1]);
					if(lhs && rhs) {
						if(auto id = std::dynamic_pointer_cast<analysis::value::identifier>(call->fn)) {
							if(id->str == "+") {
								r = std::make_shared<analysis::value::constant_integer>(lhs->val + rhs->val);
							} else if(id->str == "*") {
								r = std::make_shared<analysis::value::constant_integer>(lhs->val * rhs->val);
							}
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
			}

			val->ty = run(val->ty);

			return r;
		}

		void run(std::shared_ptr<analysis::value::module_> am) {
			for(auto &val : am->defs) {
				val = run(val);
			}
		}

		void run(analysis::unit &au) {
			for(auto &am : au.modules) {
				run(am);
			}
		}
	};
} // namespace etch::transform

#endif
