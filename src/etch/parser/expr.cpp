#include <etch/parser/config.hpp>
#include <etch/parser/module_def.hpp>

namespace etch::parser {
	BOOST_SPIRIT_INSTANTIATE(module_type, iterator_type, context_type)
} // namespace etch::parser
