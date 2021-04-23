#include <etch/mangling.hpp>
#include <sstream>

namespace etch {
	std::string mangle(std::vector<std::string> names) {
		std::ostringstream s;
		s << "etch.1";
		for(auto &name : names) {
			s << '.' << name;
		}
		return s.str();
	}
} // namespace etch

