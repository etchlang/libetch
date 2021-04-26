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
				id->ty = std::make_shared<analysis::value::type_int>(32);
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

			if(std::dynamic_pointer_cast<analysis::value::constant_integer>(val)) {
			} else if(auto id = std::dynamic_pointer_cast<analysis::value::identifier>(val)) {
				if(auto find = lookup(id->str)) {
					find->dump() << std::endl;
					auto fty = std::dynamic_pointer_cast<analysis::value::type_function>(find->ty);

					if(std::dynamic_pointer_cast<analysis::value::type_type>(find->ty)) {
						r = find;
					} else if(fty && std::dynamic_pointer_cast<analysis::value::type_type>(fty->body)) {
						r = find;
					} else {
						r->ty = find->ty;
					}
				} else if(id->str == "+" || id->str == "*") {
					auto ity = std::make_shared<analysis::value::type_int>(32);

					auto tty = std::make_shared<analysis::value::type_tuple>();
					tty->push_back(ity);
					tty->push_back(ity);

					auto fty = std::make_shared<analysis::value::type_function>(tty, ity);

					id->ty = fty;
				}
			} else if(auto intr = std::dynamic_pointer_cast<analysis::value::intrinsic>(val)) {
				if(intr->str == "int") {
					auto tyty = std::make_shared<analysis::value::type_type>();
					auto ity = std::make_shared<analysis::value::type_int>(32);
					auto fty = std::make_shared<analysis::value::type_function>(ity, tyty);
					intr->ty = fty;
				}
			} else if(auto call = std::dynamic_pointer_cast<analysis::value::call>(val)) {
				call->fn = run(call->fn);
				call->arg = run(call->arg);

				if(auto fty = std::dynamic_pointer_cast<analysis::value::type_function>(call->fn->ty)) {
					call->ty = fty->body;
				}
			} else if(auto def = std::dynamic_pointer_cast<analysis::value::definition>(val)) {
				def->val = run(def->val);
				bind(def->binding, def->val);

				def->binding->ty = def->val->ty;
				def->ty          = def->val->ty;
			} else if(auto tuple = std::dynamic_pointer_cast<analysis::value::tuple>(val)) {
				for(auto &val : tuple->vals) {
					val = run(val);
				}

				auto tty = std::make_shared<analysis::value::type_tuple>();
				for(auto &val : tuple->vals) {
					tty->push_back(val->ty);
				}
				r->ty = tty;
			} else if(auto block = std::dynamic_pointer_cast<analysis::value::block>(val)) {
				stack.emplace_back(scope{});

				for(auto &val : block->vals) {
					val = run(val);
				}

				if(!block->vals.empty()) {
					block->ty = block->vals.back()->ty;
				}

				stack.pop_back();
			} else if(auto fn = std::dynamic_pointer_cast<analysis::value::function>(val)) {
				stack.emplace_back(scope{});

				bind(fn->arg);
				fn->arg = run(fn->arg);
				fn->body = run(fn->body);

				auto fty = std::dynamic_pointer_cast<analysis::value::type_function>(fn->ty);
				fty->arg = fn->arg->ty;
				fty->body = fn->body->ty;

				stack.pop_back();
			} else if(auto m = std::dynamic_pointer_cast<analysis::value::module_>(val)) {
				stack.emplace_back(scope{});

				for(auto &def : m->defs) {
					def = run(def);
				}

				stack.pop_back();
			} else if(std::dynamic_pointer_cast<analysis::value::type_type>(val)) {
				// bail to avoid recursing with null type
				return r;
			} else if(std::dynamic_pointer_cast<analysis::value::type_unresolved>(val)) {
			} else if(std::dynamic_pointer_cast<analysis::value::type_int>(val)) {
			} else if(auto ty = std::dynamic_pointer_cast<analysis::value::type_tuple>(val)) {
				for(auto &ty_inner : ty->tys) {
					ty_inner = run(ty_inner);
				}
			} else if(auto ty = std::dynamic_pointer_cast<analysis::value::type_function>(val)) {
				ty->arg = run(ty->arg);
				ty->body = run(ty->body);
			} else if(std::dynamic_pointer_cast<analysis::value::type_module>(val)) {
			} else {
				std::ostringstream s;
				s << "analysis::resolution: unhandled value: ";
				val->dump(s);
				auto str = s.str();

				std::cerr << str << std::endl << std::endl;
				throw std::runtime_error(s.str());
			}

			r->ty = run(r->ty);

			if(std::dynamic_pointer_cast<analysis::value::type_unresolved>(r->ty)) {
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
