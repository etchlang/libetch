#ifndef ETCH_CONFIG_HPP
#define ETCH_CONFIG_HPP 1

namespace etch::parser {
	namespace x3 = boost::spirit::x3;
	using iterator_type = std::string_view::const_iterator;
	using context_type = x3::unused_type;
} // namespace etch::parser

#endif
