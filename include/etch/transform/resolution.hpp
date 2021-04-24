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

		analysis::value::ptr run(analysis::value::ptr val) {
			auto r = val;

			if(std::dynamic_pointer_cast<analysis::value::constant_integer>(val)) {
			} else if(auto id = std::dynamic_pointer_cast<analysis::value::identifier>(val)) {
				if(auto find = lookup(id->str)) {
					if(std::dynamic_pointer_cast<analysis::value::type_type>(find->ty)) {
						r = find;
					} else if(std::dynamic_pointer_cast<analysis::value::type_function>(find->ty)) {
						r = find;
					}
					val->ty = find->ty;
				} else if(id->str == "+" || id->str == "*") {
					auto ity = std::make_shared<analysis::value::type_int>(32);
					auto fty = std::make_shared<analysis::value::type_function>(ity);
					fty->push_arg(ity);
					fty->push_arg(ity);

					id->ty = fty;
				}
			} else if(auto intr = std::dynamic_pointer_cast<analysis::value::intrinsic>(val)) {
				if(intr->str == "int") {
					auto tyty = std::make_shared<analysis::value::type_type>();
					auto ity = std::make_shared<analysis::value::type_int>(32);
					auto fty = std::make_shared<analysis::value::type_function>(tyty);
					fty->push_arg(ity);

					intr->ty = fty;
				}
			} else if(auto call = std::dynamic_pointer_cast<analysis::value::call>(val)) {
				call->fn = run(call->fn);
				for(auto &arg : call->args) {
					arg = run(arg);
				}

				if(auto fty = std::dynamic_pointer_cast<analysis::value::type_function>(call->fn->ty)) {
					call->ty = fty->body;
				}
			} else if(auto def = std::dynamic_pointer_cast<analysis::value::definition>(val)) {
				def->val = run(def->val);
				def->name.ty = def->val->ty;
				def->ty      = def->val->ty;

				stack.back().syms.emplace(def->name.str, def->val);
			} else if(auto tuple = std::dynamic_pointer_cast<analysis::value::tuple>(val)) {
				for(auto &val : tuple->vals) {
					val = run(val);
				}
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

				for(auto &arg : fn->args) {
					arg->ty = std::make_shared<analysis::value::type_int>(32);

					auto id = std::dynamic_pointer_cast<analysis::value::identifier>(arg);
					stack.back().syms.emplace(id->str, arg);
				}

				fn->body = run(fn->body);

				auto fty = std::make_shared<analysis::value::type_function>(fn->body->ty);
				for(auto &arg : fn->args) {
					fty->push_arg(arg->ty);
				}
				fn->ty = fty;

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
				for(auto &arg : ty->args) {
					arg = run(arg);
				}
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

			val->ty = run(val->ty);

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
