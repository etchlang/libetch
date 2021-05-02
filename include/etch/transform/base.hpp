#ifndef ETCH_TRANSFORM_BASE_HPP
#define ETCH_TRANSFORM_BASE_HPP 1

namespace etch::transform {
	class base {
	  private:
		struct scope {
			std::unordered_map<std::string, ir::ptr<ir::base>> syms;
		};

		std::vector<scope> stack;

		void bind(ir::ptr<ir::base> binding, ir::ptr<ir::base> val) {
			if(auto id = ir::as<ir::identifier>(binding)) {
				id->resolve(std::make_shared<ir::type_int>(32));
				stack.back().syms.emplace(id->str, val);
			} else if(auto tuple = ir::as<ir::tuple>(binding)) {
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

		void bind(ir::ptr<ir::base> binding) {
			return bind(binding, binding);
		}
	  protected:
		ir::ptr<ir::base> lookup(std::string name) const {
			for(auto it = stack.rbegin(); it != stack.rend(); ++it) {
				auto search = it->syms.find(name);
				if(search != it->syms.end()) {
					return search->second;
				}
			}
			return nullptr;
		}

		virtual ir::ptr<ir::base> visit(ir::ptr<ir::constant_int>    x) { return x; }
		virtual ir::ptr<ir::base> visit(ir::ptr<ir::identifier>      x) { return x; }
		virtual ir::ptr<ir::base> visit(ir::ptr<ir::call>            x) { return x; }
		virtual ir::ptr<ir::base> visit(ir::ptr<ir::definition>      x) { return x; }
		virtual ir::ptr<ir::base> visit(ir::ptr<ir::tuple>           x) { return x; }
		virtual ir::ptr<ir::base> visit(ir::ptr<ir::block>           x) { return x; }
		virtual ir::ptr<ir::base> visit(ir::ptr<ir::function>        x) { return x; }
		virtual ir::ptr<ir::base> visit(ir::ptr<ir::module_>         x) { return x; }

		virtual ir::ptr<ir::base> visit(ir::ptr<ir::intr_int>        x) { return x; }
		virtual ir::ptr<ir::base> visit(ir::ptr<ir::intr_add>        x) { return x; }
		virtual ir::ptr<ir::base> visit(ir::ptr<ir::intr_mul>        x) { return x; }

		virtual ir::ptr<ir::base> visit(ir::ptr<ir::cast>            x) { return x; }

		virtual ir::ptr<ir::base> visit(ir::ptr<ir::type_type>       x) { return x; }
		virtual ir::ptr<ir::base> visit(ir::ptr<ir::type_unresolved> x) { return x; }
		virtual ir::ptr<ir::base> visit(ir::ptr<ir::type_int>        x) { return x; }

		virtual ir::ptr<ir::base> post(ir::ptr<ir::base> x) { return x; }
	  public:
		ir::ptr<ir::base> run(ir::ptr<ir::base> val) {
			if(val == nullptr) { return val; }

			auto r = val;

			if(auto x = ir::as<ir::constant_int>(val)) {
				r = visit(x);
			} else if(auto x = ir::as<ir::identifier>(val)) {
				r = visit(x);
			} else if(auto x = ir::as<ir::call>(val)) {
				x->fn  = run(x->fn);
				x->arg = run(x->arg);
				r = visit(x);
			} else if(auto x = ir::as<ir::definition>(val)) {
				x->val = run(x->val);
				bind(x->binding, x->val);
				r = visit(x);
			} else if(auto x = ir::as<ir::tuple>(val)) {
				for(auto &val : x->vals) {
					val = run(val);
				}
				r = visit(x);
			} else if(auto x = ir::as<ir::block>(val)) {
				stack.emplace_back(scope{});

				for(auto &val : x->vals) {
					val = run(val);
				}
				r = visit(x);

				stack.pop_back();
			} else if(auto x = ir::as<ir::function>(val)) {
				stack.emplace_back(scope{});

				x->arg  = run(x->arg);
				bind(x->arg);
				x->body = run(x->body);

				r = visit(x);

				stack.pop_back();
			} else if(auto x = ir::as<ir::module_>(val)) {
				stack.emplace_back(scope{});

				for(auto &def : x->defs) {
					def = run(def);
				}
				r = visit(x);

				stack.pop_back();
			} else if(auto x = ir::as<ir::intr_int>(val)) {
				r = visit(x);
			} else if(auto x = ir::as<ir::intr_add>(val)) {
				r = visit(x);
			} else if(auto x = ir::as<ir::intr_mul>(val)) {
				r = visit(x);
			} else if(auto x = ir::as<ir::cast>(val)) {
				x->ty  = run(x->ty);
				x->val = run(x->val);
				r = visit(x);
			} else if(ir::as<ir::type_type>(val)) {
				r = visit(x);
			} else if(auto x = ir::as<ir::type_unresolved>(val)) {
				r = visit(x);
			} else if(auto x = ir::as<ir::type_int>(val)) {
				r = visit(x);
			} else {
				std::ostringstream s;
				s << "transform::base: unhandled value: ";
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
