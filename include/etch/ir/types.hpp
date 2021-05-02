#ifndef ETCH_IR_TYPES_HPP
#define ETCH_IR_TYPES_HPP 1

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace etch::ir {
	template<typename T>
	using ptr = std::shared_ptr<T>;

	class base {
	  public:
		virtual ptr<base> type() const = 0;

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

	template<typename T>
	inline ptr<T> as(ptr<base> val) {
		return std::dynamic_pointer_cast<T>(val);
	}

	template<typename T>
	inline bool is(ptr<base> val) {
		return bool(as<T>(val));
	}

	struct cast : base {
		ptr<base> ty;
		ptr<base> val;

		cast(ptr<base> val, ptr<base> ty) : val(val), ty(ty) {}

		ptr<base> type() const {
			return ty;
		}

		std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
			s << "(cast ";
			val->dump_impl(s, depth);
			return s << ')';
		}
	};

	struct type_type : base {
		ptr<base> type() const {
			return std::make_shared<type_type>();
		}

		std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
			return s << "(type_type)";
		}
	};

	struct type_unresolved : base {
		ptr<base> type() const {
			return std::make_shared<type_type>();
		}

		std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
			return s << "(type_unresolved)";
		}
	};

	struct type_any : base {
		ptr<base> type() const {
			return std::make_shared<type_type>();
		}

		std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
			return s << "(type_any)";
		}
	};

	struct type_int : base {
		size_t width;

		type_int(size_t width) : width(width) {}

		ptr<base> type() const {
			return std::make_shared<type_type>();
		}

		std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
			return s << "(type_int " << width << ')';
		}
	};

	struct constant_int : base {
		int32_t val;
		size_t width;

		constant_int(int32_t val, size_t width = 32) : val(val), width(width) {}

		ptr<base> type() const {
			return std::make_shared<type_int>(width);
		}

		std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
			return s << "(constant_int " << val << ')';
		}
	};

	struct identifier : base {
	  private:
		ptr<base> resolved;
	  public:
		using string_type = std::string;

		string_type str;

		identifier(string_type str = "") : str(str) {}

		ptr<base> type() const {
			return resolved ? resolved : std::make_shared<type_unresolved>();
		}

		void resolve(ptr<base> ty) {
			resolved = ty;
		}

		std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
			return s << "(identifier " << str << ')';
		}
	};

	struct definition : base {
		ptr<base> binding;
		ptr<base> val;

		ptr<base> type() const {
			return val->type();
		}

		definition(ptr<base> binding, ptr<base> val) : binding(binding), val(val) {}

		std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
			s << "(definition" << std::endl;
			binding->dump(s, depth + 1) << std::endl;
			val->dump(s, depth + 1) << std::endl;
			return dump_depth(s, depth) << ')';
		}
	};

	struct tuple : base {
		std::vector<ptr<base>> vals;

		ptr<base> type() const {
			auto r = std::make_shared<tuple>();
			for(auto &val : vals) {
				r->push_back(val->type());
			}
			return r;
		}

		void push_back(ptr<base> x) {
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
		std::vector<ptr<base>> vals;

		ptr<base> type() const {
			if(vals.empty()) {
				return std::make_shared<tuple>();
			} else {
				return vals.back()->type();
			}
		}

		void push_back(ptr<base> x) {
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
		ptr<base> arg;
		ptr<base> body;

		function(ptr<base> arg, ptr<base> body) : arg(arg), body(body) {}

		ptr<base> type() const {
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
		ptr<base> fn;
		ptr<base> arg;

		call(ptr<base> fn, ptr<base> arg) : fn(fn), arg(arg) {}

		ptr<base> type() const {
			if(auto fty = as<function>(fn->type())) {
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
		std::vector<ptr<base>> defs;

		ptr<base> type() const {
			return std::make_shared<type_type>();
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
		ptr<base> type() const {
			auto tyty = std::make_shared<type_type>();
			auto ity = std::make_shared<type_int>(32);
			return std::make_shared<function>(ity, tyty);
		}

		std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
			return s << "(intr_int)";
		}
	};

	struct intr_binop : base {
		ptr<base> type() const {
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

	struct unit {
		std::vector<ptr<ir::module_>> modules;

		std::ostream & dump(std::ostream &s = std::cout, size_t depth = 0) const {
			base::dump_depth(s, depth) << "(unit" << std::endl;
			for(auto &m : modules) {
				m->dump(s, depth + 1) << std::endl;
			}
			return base::dump_depth(s, depth) << ')';
		}
	};
} // namespace etch::ir

#endif
