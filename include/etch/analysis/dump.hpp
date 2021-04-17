#ifndef ETCH_ANALYSIS_DUMP_HPP
#define ETCH_ANALYSIS_DUMP_HPP 1

#include <etch/analysis/types.hpp>
#include <iostream>
#include <stddef.h>

namespace etch::analysis {
	inline std::ostream & dump(std::ostream &s, const module_ &m, size_t depth = 0) {
		value::base::dump_depth(s, depth) << "(module" << std::endl;
		for(auto &def : m.defs) {
			def->dump(s, depth + 1) << std::endl;
		}
		return value::base::dump_depth(s, depth) << ')';
	}
} // namespace etch::analysis

#endif
