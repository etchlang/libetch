#ifndef ETCH_CODEGEN_HPP
#define ETCH_CODEGEN_HPP 1

#include <etch/analysis/types.hpp>

namespace etch {
	class codegen {
	  public:
		std::string run(const analysis::module_ &am);
	};
} // namespace etch

#endif
