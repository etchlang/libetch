#ifndef ETCH_AST_HPP
#define ETCH_AST_HPP 1

#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <cstdint>
#include <iostream>

namespace etch::ast {
	namespace x3 = boost::spirit::x3;

	struct block : std::vector<struct expr> {};

	struct tuple : std::vector<struct expr> {};

	struct identifier : std::string {};

	struct integer {
		int32_t value;
	};

	struct primary : x3::variant<
		integer,
		identifier,
		block,
		tuple
	> {
		using base_type::base_type;
		using base_type::operator=;
	};

	struct atom : x3::variant<
		primary
	> {
		using base_type::base_type;
		using base_type::operator=;
	};

	struct compound : x3::variant<
		atom,
		x3::forward_ast<struct op>
	> {
		using base_type::base_type;
		using base_type::operator=;
	};

	struct expr : x3::variant<
		compound
	> {
		using base_type::base_type;
		using base_type::operator=;
	};

	struct definition {
		identifier name;
		expr value;
	};

	struct statement : x3::variant<
		definition,
		expr
	> {
		using base_type::base_type;
		using base_type::operator=;
	};

	struct module : std::vector<statement> {};

	struct op {
		std::string opname;
		atom lhs;
		compound rhs;
	};

	// diagnostics

	template<typename... Ts>
	inline std::ostream & dump(std::ostream &s, const x3::variant<Ts...> &x) {
		boost::apply_visitor([&s](auto &&v) { dump(s, v); }, x);
		return s;
	}

	inline std::ostream & dump(std::ostream &s, const op &x) {
		s << "(op " << x.opname << ' ';
		dump(s, x.lhs);
		s << ' ';
		dump(s, x.rhs);
		s << ')';
		return s;
	}

	inline std::ostream & dump(std::ostream &s, const block &x) {
		s << "(block";
		for(auto &e : x) {
			s << ' ';
			dump(s, e);
		}
		return s << ')';
	}

	inline std::ostream & dump(std::ostream &s, const tuple &x) {
		s << "(tuple";
		for(auto &e : x) {
			s << ' ';
			dump(s, e);
		}
		return s << ')';
	}

	inline std::ostream & dump(std::ostream &s, const identifier &x) {
		return s << "(identifier " << x << ')';
	}

	inline std::ostream & dump(std::ostream &s, const integer &x) {
		return s << "(integer " << x.value << ')';
	}

	inline std::ostream & dump(std::ostream &s, const definition &x) {
		s << "(definition ";
		dump(s, x.name);
		s << ' ';
		dump(s, x.value);
		return s << ')';
	}

	inline std::ostream & dump(std::ostream &s, const module &x) {
		s << "(module";
		for(auto &e : x) {
			s << ' ';
			dump(s, e);
		}
		return s << ')';
	}
} // namespace etch::ast

#endif
