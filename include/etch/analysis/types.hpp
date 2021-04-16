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

			static std::ostream & dump_depth(std::ostream &s, size_t depth) {
				for(size_t i = 0; i < depth; ++i) { s << "| "; }
				return s;
			}

			std::ostream & dump(std::ostream &s, size_t depth = 0) const {
				dump_depth(s, depth);
				dump_impl(s, depth);
				if(ty) {
					s << " :: ";
					ty->dump_impl(s, depth);
				}
				return s;
			}

			virtual std::ostream & dump_impl(std::ostream &s, size_t depth) const = 0;
		};

		class type_type : public base {
		  public:
			type_type() : base(nullptr) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(type_type)";
			}
		};

		class type_int : public base {
		  public:
			size_t size;

			type_int(size_t size) : base(type_type{}), size(size) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(type_int " << size << ')';
			}
		};

		class constant_integer : public base {
		  public:
			int32_t val;

			constant_integer(int32_t val) : base(std::make_shared<type_int>(32)), val(val) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(constant_integer " << val << ')';
			}
		};

		class identifier : public base {
		  public:
			using string_type = std::string;
		  private:
			string_type _str;
		  public:
			identifier(string_type str = "") : base(nullptr), _str(str) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				return s << "(identifier " << str() << ')';
			}

			string_type str() const { return _str; }
		};

		class op : public base {
			std::shared_ptr<identifier> name;
			ptr lhs;
			ptr rhs;
		  public:
			op(identifier::string_type name, ptr lhs, ptr rhs) : base(lhs->ty), name(std::make_shared<identifier>(name)), lhs(lhs), rhs(rhs) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(op" << std::endl;
				name->dump(s, depth + 1) << std::endl;
				lhs->dump(s, depth + 1) << std::endl;
				rhs->dump(s, depth + 1) << std::endl;
				return dump_depth(s, depth) << ')';
			}
		};

		class definition : public base {
			std::shared_ptr<identifier> name;
			ptr val;
		  public:
			definition(identifier::string_type name, ptr val) : base(val->ty), name(std::make_shared<identifier>(name)), val(val) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(definition" << std::endl;
				name->dump(s, depth + 1) << std::endl;
				val->dump(s, depth + 1) << std::endl;
				return dump_depth(s, depth) << ')';
			}
		};

		class tuple : public base {
			std::vector<ptr> vals;
		  public:
			tuple() : base(type_int(32)) {}

			std::ostream & dump_impl(std::ostream &s, size_t depth = 0) const override {
				s << "(tuple" << std::endl;
				for(auto &val : vals) {
					val->dump(s, depth + 1) << std::endl;
				}
				return dump_depth(s, depth) << ')';
			}

			void push_back(ptr x) {
				vals.push_back(x);
			}
		};

		class block : public base {
			std::vector<ptr> vals;
		  public:
			block() : base(type_int(32)) {}

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

		class function : public base {
			std::vector<ptr> args;
			ptr body;
		  public:
			function(ptr body) : base(type_int(32)), body(body) {}

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

	struct module {
		std::vector<value::ptr> defs;
	};
} // namespace etch::analysis

#endif
