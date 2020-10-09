#ifndef ETCH_SYNTAX_DUMP_HPP
#define ETCH_SYNTAX_DUMP_HPP 1

#include <etch/syntax/types.hpp>
#include <iostream>

namespace etch::syntax {
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
} // namespace etch::syntax

#endif
