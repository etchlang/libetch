#include <etch/linker.hpp>
#include <lld/Common/Driver.h>

namespace etch {
	void linker::run(std::string output) {
		std::vector<const char *> args;
#if defined(_WIN32)
		args.emplace_back("lld-link.exe");
		for(auto &input : inputs) {
			args.emplace_back(input.data());
		}
		args.emplace_back("/subsystem:CONSOLE");
		args.emplace_back("/entry:main");

		std::string str_out("/out:" + output);
		args.emplace_back(str_out.data());

		lld::coff::link(args, false, llvm::outs(), llvm::errs());
#else
#error Linking is not supported on this platform
#endif
	}
} // namespace etch
