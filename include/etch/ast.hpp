#ifndef ETCH_AST_HPP
#define ETCH_AST_HPP 1

#include <boost/fusion/include/io.hpp>
#include <cstdint>

namespace etch::ast {
	struct expr {
		char op;
		int32_t lhs;
		int32_t rhs;
	};

	using boost::fusion::operator<<;
} // namespace etch::ast

#endif
