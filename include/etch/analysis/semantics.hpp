#ifndef ETCH_ANALYSIS_SEMANTICS_HPP
#define ETCH_ANALYSIS_SEMANTICS_HPP 1

#include <etch/analysis/dump.hpp>
#include <etch/analysis/types.hpp>
#include <etch/syntax/types.hpp>
#include <optional>
#include <string>

namespace etch::analysis {
	template<typename T>
	value::ptr analysis(const syntax::typed<T> &x) {
		auto v = analysis(x.value);
		//v->ty = type::typed;
		return v;
	}

	template<typename T>
	value::ptr analysis(const syntax::x3::forward_ast<syntax::typed<T>> &x) {
		return analysis(x.get());
	}

	template<typename... Ts>
	value::ptr analysis(const syntax::x3::variant<Ts...> &x) {
		return boost::apply_visitor([](auto &&sv) {
			return analysis(sv);
		}, x);
	}

	value::ptr analysis(const syntax::block &sb) {
		auto b = std::make_shared<value::block>();
		for(auto &x : sb) {
			b->push_back(analysis(x));
		}
		return std::static_pointer_cast<value::base>(b);
	}

	value::ptr analysis(const syntax::tuple &st) {
		auto t = std::make_shared<value::tuple>();
		for(auto &x : st) {
			t->push_back(analysis(x));
		}
		return std::static_pointer_cast<value::base>(t);
	}

	value::ptr analysis(const syntax::identifier &x) {
		return std::static_pointer_cast<value::base>(std::make_shared<value::identifier>(x));
	}

	value::ptr analysis(const syntax::integer &i) {
		return std::static_pointer_cast<value::base>(std::make_shared<value::constant_integer>(i.value));
	}

	value::ptr analysis(const syntax::function &sf) {
		auto body = analysis(sf.value);
		auto f = std::make_shared<value::function>(body);
		for(auto &x : sf.args) {
			f->push_arg(analysis(x));
		}
		return std::static_pointer_cast<value::base>(f);
	}

	value::ptr analysis(const syntax::definition &sd) {
		std::string name(sd.name);
		auto val = analysis(sd.value);
		return std::static_pointer_cast<value::base>(std::make_shared<value::definition>(name, val));
	}

	value::ptr analysis(const syntax::op &so) {
		std::string name(so.opname);
		auto lhs = analysis(so.lhs);
		auto rhs = analysis(so.rhs);
		return std::static_pointer_cast<value::base>(std::make_shared<value::op>(name, lhs, rhs));
	}

	struct def_analysis {
		using result_type = value::ptr;

		result_type operator()(const syntax::expr &) const {
			return std::shared_ptr<value::definition>();
		}

		result_type operator()(const syntax::x3::forward_ast<syntax::definition> &ptr) const {
			auto sdef = ptr.get();

			return analysis(ptr.get());
		}
	};

	module analysis(const syntax::module &sm) {
		module m;
		for(auto &st : sm) {
			if(auto r = boost::apply_visitor(def_analysis{}, st)) {
				m.defs.push_back(r);
			}
		}
		return m;
	}
} // namespace etch::semantics

#endif
