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
			ptr ty;

			base(ptr ty) : ty(ty) {}
			base(nullptr_t) {}
			template<typename T> base(T ty) { setTy(ty); }

			template<typename T>
			ptr setTy(T newTy) {
				return ty = std::static_pointer_cast<base>(std::make_shared<T>(newTy));
			}

			static std::ostream & dump_depth(std::ostream &s, size_t depth) {
				for(size_t i = 0; i < depth; ++i) { s << "| "; }
				return s;
			}

			std::ostream & dump(std::ostream &s = std::cout, size_t depth = 0) const {
				dump_depth(s, depth);

				if(depth > 5) {
					s << "...";
				} else {
					dump_impl(s, depth);
				}

				if(ty) {
					s << " :: ";
					ty->dump_impl(s, depth);
				}
				return s;
			}

			virtual std::ostream & dump_impl(std::ostream &s, size_t depth) const = 0;
		};

		struct type_type : base {
			type_type() : base(nullptr) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(type_type)";
			}
		};

		struct type_any : base {
			type_any() : base(type_type{}) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(type_any)";
			}
		};

		struct type_int : base {
			size_t width;

			type_int(size_t width) : base(type_type{}), width(width) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(type_int " << width << ')';
			}
		};

		struct type_tuple : base {
			std::vector<ptr> tys;

			type_tuple() : base(type_type{}) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(type_tuple" << std::endl;
				for(auto &ty : tys) {
					if(ty) {
						ty->dump(s, depth + 4) << std::endl;
					} else {
						dump_depth(s, depth + 4) << "???" << std::endl;
					}
				}
				return dump_depth(s, depth) << ')';
			}
		};

		struct type_function : base {
			ptr lhs;
			ptr body;

			type_function(ptr lhs, ptr body) : base(type_type{}), lhs(lhs), body(body) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(type_function ";

				if(lhs) {
					lhs->dump_impl(s, depth) << ' ';
				} else {
					s << "???";
				}

				if(body) {
					body->dump_impl(s, depth);
				} else {
					s << "???";
				}

				return s << ')';
			}
		};

		struct constant_integer : base {
			int32_t val;

			constant_integer(int32_t val) : base(type_int{32}), val(val) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(constant_integer " << val << ')';
			}
		};

		struct identifier : base {
			using string_type = std::string;

			string_type str;

			identifier(string_type str = "") : base(nullptr), str(str) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(identifier " << str << ')';
			}
		};

		struct op : base {
			std::shared_ptr<identifier> name;
			ptr lhs;
			ptr rhs;

			op(identifier::string_type name, ptr lhs, ptr rhs) : base(lhs->ty), name(std::make_shared<identifier>(name)), lhs(lhs), rhs(rhs) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(op" << std::endl;
				name->dump(s, depth + 1) << std::endl;
				lhs->dump(s, depth + 1) << std::endl;
				rhs->dump(s, depth + 1) << std::endl;
				return dump_depth(s, depth) << ')';
			}
		};

		struct definition : base {
			identifier name;
			ptr val;

			definition(identifier::string_type name, ptr val) : base(val->ty), name(name), val(val) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(definition" << std::endl;
				name.dump(s, depth + 1) << std::endl;
				val->dump(s, depth + 1) << std::endl;
				return dump_depth(s, depth) << ')';
			}
		};

		struct tuple : base {
			std::vector<ptr> vals;

			tuple() : base(type_tuple{}) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(tuple" << std::endl;
				for(auto &val : vals) {
					val->dump(s, depth + 1) << std::endl;
				}
				return dump_depth(s, depth) << ')';
			}

			void push_back(ptr x) {
				vals.push_back(x);
				auto ty_tuple = std::dynamic_pointer_cast<type_tuple>(ty);
				ty_tuple->tys.push_back(x->ty);
			}
		};

		struct block : base {
			std::vector<ptr> vals;

			block() : base(type_tuple{}) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(block" << std::endl;
				for(auto &val : vals) {
					val->dump(s, depth + 1) << std::endl;
				}
				return dump_depth(s, depth) << ')';
			}

			void push_back(ptr x) {
				vals.push_back(x);
				ty = x->ty;
			}
		};

		struct function : base {
			std::vector<ptr> args;
			ptr body;

			function(ptr body) : base(type_function(std::make_shared<type_any>(), body->ty)), body(body) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(function" << std::endl;
				for(auto &arg : args) {
					arg->dump(s, depth + 1) << std::endl;
				}
				body->dump(s, depth + 1) << std::endl;
				return dump_depth(s, depth) << ')';
			}

			void push_arg(ptr x) {
				args.push_back(x);
			}
		};
	} // namspace value

	struct module_ {
		std::vector<value::ptr> defs;

		std::ostream & dump(std::ostream &s = std::cout, size_t depth = 0) const {
			value::base::dump_depth(s, depth) << "(module" << std::endl;
			for(auto &def : defs) {
				def->dump(s, depth + 1) << std::endl;
			}
			return value::base::dump_depth(s, depth) << ')';
		}
	};
} // namespace etch::analysis

#endif
