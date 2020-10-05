#include <etch/expr_def.hpp>
#include <etch/config.hpp>

namespace etch::parser {
	BOOST_SPIRIT_INSTANTIATE(expr_type, iterator_type, context_type)
} // namespace etch::parser
