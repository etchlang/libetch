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
		value::ptr run(const syntax::typed<T> &x) {
			auto v = run(x.value);
			v->ty = run(x.type);
			return v;
		}

		template<typename T>
		value::ptr run(const syntax::x3::forward_ast<syntax::typed<T>> &x) {
			return run(x.get());
		}

		template<typename... Ts>
		value::ptr run(const syntax::x3::variant<Ts...> &x) {
			return boost::apply_visitor([this](auto &&sv) {
				return run(sv);
			}, x);
		}

		value::ptr run(const syntax::block &sb) {
			auto b = std::make_shared<value::block>();
			for(auto &x : sb) {
				b->push_back(run(x));
			}
			return std::static_pointer_cast<value::base>(b);
		}

		value::ptr run(const syntax::tuple &st) {
			auto t = std::make_shared<value::tuple>();
			for(auto &x : st) {
				t->push_back(run(x));
			}
			return std::static_pointer_cast<value::base>(t);
		}

		value::ptr run(const syntax::identifier &x) {
			return std::static_pointer_cast<value::base>(std::make_shared<value::identifier>(x));
		}

		value::ptr run(const syntax::integer &i) {
			return std::static_pointer_cast<value::base>(std::make_shared<value::constant_integer>(i.value));
		}

		value::ptr run(const syntax::function &sf) {
			auto body = run(sf.value);
			auto f = std::make_shared<value::function>(body);
			for(auto &x : sf.args) {
				auto arg = run(x);
				if(!arg->ty) {
					arg->setTy(value::type_any{});
				}
				f->push_arg(arg);
			}
			return std::static_pointer_cast<value::base>(f);
		}

		value::ptr run(const syntax::definition &sd) {
			std::string name(sd.name);
			auto val = run(sd.value);
			return std::static_pointer_cast<value::base>(std::make_shared<value::definition>(name, val));
		}

		value::ptr run(const syntax::op &so) {
			std::string name(so.opname);
			auto lhs = run(so.lhs);
			auto rhs = run(so.rhs);
			return std::static_pointer_cast<value::base>(std::make_shared<value::op>(name, lhs, rhs));
		}

		module_ run(const syntax::module &sm) {
			module_ m;
			for(auto &st : sm) {
				m.defs.emplace_back(run(st));
			}
			return m;
		}

		unit run(const syntax::unit &su) {
			unit u;
			for(auto &sm : su) {
				u.modules.emplace_back(run(sm));
			}
			return u;
		}
	};
} // namespace etch::analysis

#endif
