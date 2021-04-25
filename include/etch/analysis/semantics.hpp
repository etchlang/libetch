#ifndef ETCH_ANALYSIS_SEMANTICS_HPP
#define ETCH_ANALYSIS_SEMANTICS_HPP 1

#include <etch/analysis/types.hpp>
#include <etch/syntax/types.hpp>
#include <optional>
#include <string>

namespace etch::analysis {
	class semantics {
	  public:
		template<typename T>
		value::ptr visit(const syntax::typed<T> &x) {
			auto v = visit(x.value);
			v->ty = visit(x.type);
			return v;
		}

		template<typename T>
		value::ptr visit(const syntax::x3::forward_ast<syntax::typed<T>> &x) {
			return visit(x.get());
		}

		template<typename... Ts>
		value::ptr visit(const syntax::x3::variant<Ts...> &x) {
			return boost::apply_visitor([this](auto &&sv) {
				return visit(sv);
			}, x);
		}

		value::ptr visit(const syntax::block &sb) {
			auto b = std::make_shared<value::block>();
			for(auto &x : sb) {
				b->push_back(visit(x));
			}
			return std::static_pointer_cast<value::base>(b);
		}

		value::ptr visit(const syntax::tuple &st) {
			auto t = std::make_shared<value::tuple>();
			for(auto &x : st) {
				t->push_back(visit(x));
			}
			return std::static_pointer_cast<value::base>(t);
		}

		value::ptr visit(const syntax::identifier &x) {
			return std::static_pointer_cast<value::base>(std::make_shared<value::identifier>(x));
		}

		value::ptr visit(const syntax::intrinsic &x) {
			return std::static_pointer_cast<value::base>(std::make_shared<value::intrinsic>(x));
		}

		value::ptr visit(const syntax::integer &i) {
			return std::static_pointer_cast<value::base>(std::make_shared<value::constant_integer>(i.value));
		}

		value::ptr visit(const syntax::function &sf) {
			auto arg = visit(sf.arg);
			auto body = visit(sf.body);
			auto f = std::make_shared<value::function>(arg, body);
			return std::static_pointer_cast<value::base>(f);
		}

		value::ptr visit(const syntax::definition &sd) {
			auto binding = visit(sd.binding);
			auto val = visit(sd.value);
			return std::static_pointer_cast<value::base>(std::make_shared<value::definition>(binding, val));
		}

		value::ptr visit(const syntax::op &so) {
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

			return std::static_pointer_cast<value::base>(c);
		}

		std::shared_ptr<value::module_> visit(const syntax::module &sm) {
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
