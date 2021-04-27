#ifndef ETCH_TRANSFORM_RESOLUTION_HPP
#define ETCH_TRANSFORM_RESOLUTION_HPP 1

namespace etch::transform {
	class resolution {
		struct scope {
			std::unordered_map<std::string, analysis::value::ptr> syms;
		};

		std::vector<scope> stack;
	  public:
		analysis::value::ptr lookup(std::string name) const {
			for(auto it = stack.rbegin(); it != stack.rend(); ++it) {
				auto search = it->syms.find(name);
				if(search != it->syms.end()) {
					return search->second;
				}
			}
			return nullptr;
		}

		void bind(analysis::value::ptr binding, analysis::value::ptr val) {
			if(auto id = std::dynamic_pointer_cast<analysis::value::identifier>(binding)) {
				id->resolve(std::make_shared<analysis::value::type_int>(32));
				stack.back().syms.emplace(id->str, val);
			} else if(auto tuple = std::dynamic_pointer_cast<analysis::value::tuple>(binding)) {
				for(auto &val : tuple->vals) {
					bind(val);
				}
			} else {
				std::ostringstream s;
				s << "analysis::resolution: unhandled binding: ";
				binding->dump(s);
				auto str = s.str();

				std::cerr << str << std::endl << std::endl;
				throw std::runtime_error(s.str());
			}
		}

		void bind(analysis::value::ptr binding) {
			return bind(binding, binding);
		}

		analysis::value::ptr run(analysis::value::ptr val) {
			auto r = val;

			if(std::dynamic_pointer_cast<analysis::value::constant_int>(val)) {
			} else if(auto id = std::dynamic_pointer_cast<analysis::value::identifier>(val)) {
				if(auto find = lookup(id->str)) {
					auto find_ty = find->type();
					auto fty = std::dynamic_pointer_cast<analysis::value::function>(find->type());

					if(std::dynamic_pointer_cast<analysis::value::type_type>(find_ty)) {
						r = find;
					} else if(fty && std::dynamic_pointer_cast<analysis::value::type_type>(fty->body)) {
						r = find;
					} else {
						id->resolve(find->type());
					}
				} else if(id->str == "+") {
					r = std::make_shared<analysis::value::intr_add>();
				} else if(id->str == "*") {
					r = std::make_shared<analysis::value::intr_mul>();
				}
			} else if(auto call = std::dynamic_pointer_cast<analysis::value::call>(val)) {
				call->fn = run(call->fn);
				call->arg = run(call->arg);
			} else if(auto def = std::dynamic_pointer_cast<analysis::value::definition>(val)) {
				def->val = run(def->val);
				bind(def->binding, def->val);
			} else if(auto tuple = std::dynamic_pointer_cast<analysis::value::tuple>(val)) {
				for(auto &val : tuple->vals) {
					val = run(val);
				}
			} else if(auto block = std::dynamic_pointer_cast<analysis::value::block>(val)) {
				stack.emplace_back(scope{});

				for(auto &val : block->vals) {
					val = run(val);
				}

				stack.pop_back();
			} else if(auto fn = std::dynamic_pointer_cast<analysis::value::function>(val)) {
				stack.emplace_back(scope{});

				bind(fn->arg);
				fn->arg = run(fn->arg);
				fn->body = run(fn->body);

				stack.pop_back();
			} else if(auto m = std::dynamic_pointer_cast<analysis::value::module_>(val)) {
				stack.emplace_back(scope{});

				for(auto &def : m->defs) {
					def = run(def);
				}

				stack.pop_back();
			} else if(auto cast = std::dynamic_pointer_cast<analysis::value::cast>(val)) {
				cast->ty  = run(cast->ty);
				cast->val = run(cast->val);
			} else if(std::dynamic_pointer_cast<analysis::value::type_type>(val)) {
				// bail to avoid infinite loop
				return r;
			} else if(std::dynamic_pointer_cast<analysis::value::type_unresolved>(val)) {
			} else if(std::dynamic_pointer_cast<analysis::value::type_int>(val)) {
			} else if(std::dynamic_pointer_cast<analysis::value::type_module>(val)) {
			} else if(std::dynamic_pointer_cast<analysis::value::intr_int>(val)) {
			} else {
				std::ostringstream s;
				s << "analysis::resolution: unhandled value: ";
				val->dump(s);
				auto str = s.str();

				std::cerr << str << std::endl << std::endl;
				throw std::runtime_error(s.str());
			}

			if(std::dynamic_pointer_cast<analysis::value::type_unresolved>(r->type())) {
				std::ostringstream s;
				s << "analysis::resolution: unresolved type: ";
				r->dump(s);
				auto str = s.str();

				std::cerr << str << std::endl << std::endl;
			}

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
