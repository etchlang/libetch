#include <etch/mangling.hpp>

namespace etch {
	std::string mangle(std::string name) {
		return "etch.1." + name;
	}
} // namespace etch

