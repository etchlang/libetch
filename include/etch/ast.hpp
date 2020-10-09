#ifndef ETCH_AST_HPP
#define ETCH_AST_HPP 1

#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <cstdint>
#include <iostream>

namespace etch::ast {
	namespace x3 = boost::spirit::x3;

	template<typename> struct typed;

	struct block : std::vector<struct statement> {};

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
		x3::forward_ast<struct function>,
		x3::forward_ast<struct call>
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

	using arg = atom;
	struct arglist : std::vector<arg> {};

	struct function {
		arglist args;
		expr value;
	};

	struct call {
		atom callable;
		expr arg;
	};

	struct definition {
		identifier name;
		expr value;
	};

	struct op {
		std::string opname;
		atom lhs;
		compound rhs;
	};

	template<typename T>
	struct typed {
		T value;
		atom type;
	};

	// diagnostics

	inline std::ostream & dump_depth(std::ostream &s, size_t depth) {
		for(size_t i = 0; i < depth; ++i) { s << "| "; }
		return s;
	}

	template<typename T>
	inline std::ostream & dump(std::ostream &s, const typed<T> &x, size_t depth = 0) {
		dump_depth(s, depth) << "(typed" << std::endl;
		dump(s, x.value, depth + 1) << std::endl;
		dump(s, x.type, depth + 1) << std::endl;
		return dump_depth(s, depth) << ')';
	}

	template<typename T>
	inline std::ostream & dump(std::ostream &s, const x3::forward_ast<typed<T>> &x, size_t depth = 0) {
		return dump(s, x.get(), depth);
	}

	template<typename... Ts>
	inline std::ostream & dump(std::ostream &s, const x3::variant<Ts...> &x, size_t depth = 0) {
		boost::apply_visitor([&s, &depth](auto &&v) {
			dump(s, v, depth);
		}, x);
		return s;
	}

	inline std::ostream & dump(std::ostream &s, const op &x, size_t depth = 0) {
		dump_depth(s, depth) << "(op" << std::endl;
		dump_depth(s, depth + 1) << x.opname << std::endl;
		dump(s, x.lhs, depth + 1) << std::endl;
		dump(s, x.rhs, depth + 1) << std::endl;
		return dump_depth(s, depth) << ')';
	}

	inline std::ostream & dump(std::ostream &s, const block &x, size_t depth = 0) {
		dump_depth(s, depth) << "(block" << std::endl;
		for(auto &e : x) {
			dump(s, e, depth + 1) << std::endl;
		}
		return dump_depth(s, depth) << ')';
	}

	inline std::ostream & dump(std::ostream &s, const tuple &x, size_t depth = 0) {
		dump_depth(s, depth) << "(tuple" << std::endl;
		for(auto &e : x) {
			dump(s, e, depth + 1) << std::endl;
		}
		return dump_depth(s, depth) << ')';
	}

	inline std::ostream & dump(std::ostream &s, const identifier &x, size_t depth = 0) {
		return dump_depth(s, depth) << "(identifier " << x << ')';
	}

	inline std::ostream & dump(std::ostream &s, const integer &x, size_t depth = 0) {
		return dump_depth(s, depth) << "(integer " << x.value << ')';
	}

	inline std::ostream & dump(std::ostream &s, const definition &x, size_t depth = 0) {
		dump_depth(s, depth) << "(definition" << std::endl;
		dump(s, x.name, depth + 1) << std::endl;
		dump(s, x.value, depth + 1) << std::endl;
		return dump_depth(s, depth) << ')';
	}

	inline std::ostream & dump(std::ostream &s, const function &x, size_t depth = 0) {
		dump_depth(s, depth) << "(function" << std::endl;
		for(auto &e : x.args) {
			dump(s, e, depth + 1) << std::endl;
		}
		dump(s, x.value, depth + 1) << std::endl;
		return dump_depth(s, depth) << ')';
	}

	inline std::ostream & dump(std::ostream &s, const call &x, size_t depth = 0) {
		dump_depth(s, depth) << "(call" << std::endl;
		dump(s, x.callable, depth + 1) << std::endl;
		dump(s, x.arg, depth + 1) << std::endl;
		return dump_depth(s, depth) << ')';
	}

	inline std::ostream & dump(std::ostream &s, const module &x, size_t depth = 0) {
		dump_depth(s, depth) << "(module" << std::endl;
		for(auto &e : x) {
			dump(s, e, depth + 1) << std::endl;
		}
		return dump_depth(s, depth) << ')';
	}
} // namespace etch::ast

#endif
