#include <etch/parser/config.hpp>
#include <etch/parser/unit_def.hpp>

namespace etch::parser {
	BOOST_SPIRIT_INSTANTIATE(unit_type, iterator_type, context_type)
} // namespace etch::parser
