#ifndef ETCH_ANALYSIS_TYPES_HPP
#define ETCH_ANALYSIS_TYPES_HPP 1

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace etch::analysis {
	namespace value {
		using ptr = std::shared_ptr<class base>;

		class base {
		  public:
			virtual ptr type() const = 0;

			static std::ostream & dump_depth(std::ostream &s, size_t depth) {
				for(size_t i = 0; i < depth; ++i) { s << "| "; }
				return s;
			}

			std::ostream & dump(std::ostream &s = std::cout, size_t depth = 0) {
				dump_depth(s, depth);

				if(depth > 8) {
					s << "...";
				} else {
					dump_impl(s, depth);
				}

				s << " :: ";
				type()->dump_impl(s, depth);

				return s;
			}

			virtual std::ostream & dump_impl(std::ostream &s, size_t depth) const = 0;
		};

		struct cast : base {
			ptr ty;
			ptr val;

			cast(ptr val, ptr ty) : val(val), ty(ty) {}

			ptr type() const {
				return ty;
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(cast ";
				val->dump_impl(s, depth);
				return s << ')';
			}
		};

		struct type_type : base {
			ptr type() const {
				return std::make_shared<type_type>();
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(type_type)";
			}
		};

		struct type_unresolved : base {
			ptr type() const {
				return std::make_shared<type_type>();
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(type_unresolved)";
			}
		};

		struct type_any : base {
			ptr type() const {
				return std::make_shared<type_type>();
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(type_any)";
			}
		};

		struct type_int : base {
			size_t width;

			type_int(size_t width) : width(width) {}

			ptr type() const {
				return std::make_shared<type_type>();
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(type_int " << width << ')';
			}
		};

		struct type_module : base {
			ptr type() const {
				return std::make_shared<type_type>();
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(type_module)";
			}
		};

		struct constant_int : base {
			int32_t val;
			size_t width;

			constant_int(int32_t val, size_t width = 32) : val(val), width(width) {}

			ptr type() const {
				return std::make_shared<type_int>(width);
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(constant_int " << val << ')';
			}
		};

		struct identifier : base {
		  private:
			ptr resolved;
		  public:
			using string_type = std::string;

			string_type str;

			identifier(string_type str = "") : str(str) {}

			ptr type() const {
				return resolved ? resolved : std::make_shared<type_unresolved>();
			}

			void resolve(ptr ty) {
				resolved = ty;
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(identifier " << str << ')';
			}
		};

		struct intrinsic : base {
			using string_type = std::string;

			string_type str;

			intrinsic(string_type str) : str(str) {}

			ptr type() const {
				return std::make_shared<type_unresolved>();
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(intrinsic " << str << ')';
			}
		};

		struct definition : base {
			ptr binding;
			ptr val;

			ptr type() const {
				return val->type();
			}

			definition(ptr binding, ptr val) : binding(binding), val(val) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(definition" << std::endl;
				binding->dump(s, depth + 1) << std::endl;
				val->dump(s, depth + 1) << std::endl;
				return dump_depth(s, depth) << ')';
			}
		};

		struct tuple : base {
			std::vector<ptr> vals;

			ptr type() const {
				auto r = std::make_shared<tuple>();
				for(auto &val : vals) {
					r->push_back(val->type());
				}
				return r;
			}

			void push_back(ptr x) {
				vals.push_back(x);
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(tuple";
				if(!vals.empty()) {
					s << std::endl;
					for(auto &val : vals) {
						val->dump(s, depth + 1) << std::endl;
					}
					dump_depth(s, depth);
				}
				return s << ')';
			}
		};

		struct block : base {
			std::vector<ptr> vals;

			ptr type() const {
				if(vals.empty()) {
					return std::make_shared<tuple>();
				} else {
					return vals.back()->type();
				}
			}

			void push_back(ptr x) {
				vals.push_back(x);
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(block";
				if(!vals.empty()) {
					s << std::endl;
					for(auto &val : vals) {
						val->dump(s, depth + 1) << std::endl;
					}
					dump_depth(s, depth);
				}
				return s << ')';
			}
		};

		struct function : base {
			ptr arg;
			ptr body;

			function(ptr arg, ptr body) : arg(arg), body(body) {}

			ptr type() const {
				return std::make_shared<function>(arg->type(), body->type());
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(function" << std::endl;
				arg->dump(s, depth + 1) << std::endl;
				body->dump(s, depth + 1) << std::endl;
				return dump_depth(s, depth) << ')';
			}
		};

		struct call : base {
			ptr fn;
			ptr arg;

			call(ptr fn, ptr arg) : fn(fn), arg(arg) {}

			ptr type() const {
				if(auto fty = std::dynamic_pointer_cast<function>(fn->type())) {
					return fty->body;
				} else {
					return std::make_shared<type_unresolved>();
				}
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(call" << std::endl;
				fn->dump(s, depth + 1) << std::endl;
				arg->dump(s, depth + 1) << std::endl;
				return dump_depth(s, depth) << ')';
			}
		};

		struct module_ : base {
			std::vector<ptr> defs;

			ptr type() const {
				return std::make_shared<type_module>();
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(module" << std::endl;
				for(auto &def : defs) {
					def->dump(s, depth + 1) << std::endl;
				}
				return dump_depth(s, depth) << ')';
			}
		};

		struct intr_int : base {
			ptr type() const {
				auto tyty = std::make_shared<type_type>();
				auto ity = std::make_shared<type_int>(32);
				return std::make_shared<function>(ity, tyty);
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(intr_int)";
			}
		};

		struct intr_binop : base {
			ptr type() const {
				auto ity = std::make_shared<type_int>(32);

				auto tty = std::make_shared<tuple>();
				tty->push_back(ity);
				tty->push_back(ity);

				return std::make_shared<function>(tty, ity);
			}
		};

		struct intr_add : intr_binop {
			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(intr_add)";
			}
		};

		struct intr_mul : intr_binop {
			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(intr_mul)";
			}
		};
	} // namspace value

	struct unit {
		std::vector<std::shared_ptr<value::module_>> modules;

		std::ostream & dump(std::ostream &s = std::cout, size_t depth = 0) const {
			value::base::dump_depth(s, depth) << "(unit" << std::endl;
			for(auto &m : modules) {
				m->dump(s, depth + 1) << std::endl;
			}
			return value::base::dump_depth(s, depth) << ')';
		}
	};
} // namespace etch::analysis

#endif
