#ifndef ETCH_SYNTAX_TYPES_HPP
#define ETCH_SYNTAX_TYPES_HPP 1

#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace etch::syntax {
	namespace x3 = boost::spirit::x3;

	template<typename> struct typed;

	struct block : std::vector<struct statement> {};

	struct tuple : std::vector<struct expr> {};

	struct identifier : std::string {};
	struct intrinsic : std::string {};

	struct integer {
		int32_t value;
	};

	struct primary : x3::variant<
		integer,
		identifier,
		intrinsic,
		block,
		tuple
	> {
		using base_type::base_type;
		using base_type::operator=;
	};

	struct atom : x3::variant<
		primary,
		x3::forward_ast<typed<primary>>
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
		compound,
		x3::forward_ast<struct module>,
		x3::forward_ast<struct function>
	> {
		using base_type::base_type;
		using base_type::operator=;
	};

	struct statement : x3::variant<
		expr,
		x3::forward_ast<struct definition>
	> {
		using base_type::base_type;
		using base_type::operator=;
	};

	struct module : std::vector<statement> {};

	struct unit : std::vector<module> {};

	struct function {
		atom arg;
		expr body;
	};

	struct definition {
		atom binding;
		expr value;
	};

	struct op {
		identifier opname;
		atom lhs;
		expr rhs;
	};

	template<typename T>
	struct typed {
		T value;
		atom type;
	};
} // namespace etch::syntax

#endif
