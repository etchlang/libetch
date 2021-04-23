#ifndef ETCH_LINKER_HPP
#define ETCH_LINKER_HPP 1

#include <string>
#include <vector>

namespace etch {
	class linker {
		std::vector<std::string> inputs;
	  public:
		void push_back(std::string input) {
			inputs.emplace_back(input);
		}

		void run(std::string = "a.out");
	};
} // namespace etch

#endif
