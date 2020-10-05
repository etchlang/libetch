#ifndef ETCH_AST_HPP
#define ETCH_AST_HPP 1

#include <cstdint>
#include <iostream>

namespace etch::ast {
	struct expr {
		char op;
		int32_t lhs;
		int32_t rhs;

		friend std::ostream & operator<<(std::ostream &s, const expr &x) {
			return s << x.op << ' ' << x.lhs << ' ' << x.rhs;
		}
	};
} // namespace etch::ast

#endif
