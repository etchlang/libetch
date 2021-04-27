#ifndef ETCH_ANALYSIS_SEMANTICS_HPP
#define ETCH_ANALYSIS_SEMANTICS_HPP 1

#include <etch/analysis/types.hpp>
#include <etch/syntax/types.hpp>
#include <optional>
#include <sstream>
#include <string>

namespace etch::analysis {
	class semantics {
	  public:
		template<typename T>
		auto visit(const syntax::typed<T> &x) {
			auto val = visit(x.value);
			auto ty  = visit(x.type);
			return std::make_shared<value::cast>(val, ty);
		}

		template<typename T>
		auto visit(const syntax::x3::forward_ast<syntax::typed<T>> &x) {
			return visit(x.get());
		}

		template<typename... Ts>
		value::ptr visit(const syntax::x3::variant<Ts...> &x) {
			return boost::apply_visitor([this](auto &&sv) {
				return std::static_pointer_cast<value::base>(visit(sv));
			}, x);
		}

		auto visit(const syntax::block &sb) {
			auto b = std::make_shared<value::block>();
			for(auto &x : sb) {
				b->push_back(visit(x));
			}
			return b;
		}

		auto visit(const syntax::tuple &st) {
			auto t = std::make_shared<value::tuple>();
			for(auto &x : st) {
				t->push_back(visit(x));
			}
			return t;
		}

		auto visit(const syntax::identifier &x) {
			return std::make_shared<value::identifier>(x);
		}

		auto visit(const syntax::intrinsic &x) {
			value::ptr r = nullptr;

			if(x == "int") {
				r = std::make_shared<analysis::value::intr_int>();
			} else {
				std::ostringstream s;
				s << "analysis::semantics: unknown intrinsic: " << x;
				auto str = s.str();

				std::cerr << str << std::endl << std::endl;
				throw std::runtime_error(s.str());
			}

			return r;
		}

		auto visit(const syntax::integer &i) {
			return std::make_shared<value::constant_int>(i.value);
		}

		auto visit(const syntax::function &sf) {
			auto arg = visit(sf.arg);
			auto body = visit(sf.body);
			return std::make_shared<value::function>(arg, body);
		}

		auto visit(const syntax::definition &sd) {
			auto binding = visit(sd.binding);
			auto val = visit(sd.value);
			return std::make_shared<value::definition>(binding, val);
		}

		auto visit(const syntax::op &so) {
			std::shared_ptr<value::call> c;

			auto lhs = visit(so.lhs);
			auto rhs = visit(so.rhs);

			if(so.opname == "<-") {
				c = std::make_shared<value::call>(lhs, rhs);
			} else {
				auto t = std::make_shared<value::tuple>();
				t->push_back(lhs);
				t->push_back(rhs);

				c = std::make_shared<value::call>(visit(so.opname), t);
			}

			return c;
		}

		auto visit(const syntax::module &sm) {
			auto m = std::make_shared<value::module_>();
			for(auto &st : sm) {
				m->defs.emplace_back(visit(st));
			}
			return m;
		}

		unit run(const syntax::unit &su) {
			unit u;
			for(auto &sm : su) {
				u.modules.emplace_back(visit(sm));
			}
			return u;
		}
	};
} // namespace etch::analysis

#endif
