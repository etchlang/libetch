#ifndef ETCH_CODEGEN_HPP
#define ETCH_CODEGEN_HPP 1

#include <etch/analysis/types.hpp>
#include <llvm/IR/Module.h>

namespace etch {
	void codegen(const analysis::module_ &am);
} // namespace etch

#endif
