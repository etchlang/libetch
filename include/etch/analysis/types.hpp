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

				if(depth > 8) {
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

		struct type_unresolved : base {
			type_unresolved() : base(type_type{}) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(type_unresolved)";
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

			void push_back(ptr x) {
				tys.emplace_back(x);
			}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(type_tuple";
				if(!tys.empty()) {
					s << std::endl;
					for(auto &ty : tys) {
						if(ty) {
							ty->dump(s, depth + 4) << std::endl;
						} else {
							dump_depth(s, depth + 4) << "???" << std::endl;
						}
					}
					dump_depth(s, depth);
				}
				return s << ')';
			}
		};

		struct type_function : base {
			ptr arg;
			ptr body;

			type_function(ptr arg, ptr body) : base(type_type{}), arg(arg), body(body) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(type_function ";
				arg->dump_impl(s, depth) << ' ';
				body->dump_impl(s, depth);
				return s << ')';
			}
		};

		struct type_module : base {
			type_module() : base(type_type{}) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(type_module)";
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

			identifier(string_type str = "") : base(type_unresolved{}), str(str) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(identifier " << str << ')';
			}
		};

		struct intrinsic : base {
			using string_type = std::string;

			string_type str;

			intrinsic(string_type str) : base(type_unresolved{}), str(str) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(intrinsic " << str << ')';
			}
		};

		struct call : base {
			ptr fn;
			ptr arg;

			call(ptr fn, ptr arg) : base(fn->ty), fn(fn), arg(arg) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(call" << std::endl;
				fn->dump(s, depth + 1) << std::endl;
				arg->dump(s, depth + 1) << std::endl;
				return dump_depth(s, depth) << ')';
			}
		};

		struct definition : base {
			ptr binding;
			ptr val;

			definition(ptr binding, ptr val) : base(val->ty), binding(binding), val(val) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(definition" << std::endl;
				binding->dump(s, depth + 1) << std::endl;
				val->dump(s, depth + 1) << std::endl;
				return dump_depth(s, depth) << ')';
			}
		};

		struct tuple : base {
			std::vector<ptr> vals;

			tuple() : base(type_tuple{}) {}

			void push_back(ptr x) {
				vals.push_back(x);
				auto ty_tuple = std::dynamic_pointer_cast<type_tuple>(ty);
				ty_tuple->tys.push_back(x->ty);
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

			block() : base(type_tuple{}) {}

			void push_back(ptr x) {
				vals.push_back(x);
				ty = x->ty;
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

			function(ptr arg, ptr body) : base(type_function(arg->ty, body->ty)), arg(arg), body(body) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(function" << std::endl;
				arg->dump(s, depth + 1) << std::endl;
				body->dump(s, depth + 1) << std::endl;
				return dump_depth(s, depth) << ')';
			}
		};

		struct module_ : base {
			std::vector<ptr> defs;

			module_() : base(type_module{}) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(module" << std::endl;
				for(auto &def : defs) {
					def->dump(s, depth + 1) << std::endl;
				}
				return dump_depth(s, depth) << ')';
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
