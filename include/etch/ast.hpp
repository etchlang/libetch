#ifndef ETCH_AST_HPP
#define ETCH_AST_HPP 1

#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <cstdint>
#include <iostream>

namespace etch::ast {
	namespace x3 = boost::spirit::x3;

	struct op;

	struct primary : x3::variant<
		int32_t
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
		x3::forward_ast<op>
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

	struct op {
		std::string opname;
		atom lhs;
		compound rhs;
	};

	inline std::ostream & dump(std::ostream &s, const int32_t &x) {
		return s << "(number " << x << ')';
	}

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
} // namespace etch::ast

#endif
