#include <etch/analysis/semantics.hpp>
#include <etch/codegen.hpp>
#include <etch/compiler.hpp>
#include <etch/parser.hpp>
#include <etch/transform/fold.hpp>
#include <etch/transform/resolution.hpp>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>

namespace etch {
	std::string compiler::run(std::string_view sv) {
		auto sm = parse(sv);
		auto am = analysis::semantics{}.run(sm);

		transform::fold{}.run(am);
		transform::resolution{}.run(am);

		codegen{ctx, m}.run(am);

		llvm::verifyModule(*m, &llvm::errs());

		// output LLVM assembly to string

		if(tgt == target::llvm_assembly) {
			std::string ll;
			llvm::raw_string_ostream os(ll);
			os << *m;
			os.flush();

			return ll;
		}

		// emit object

		llvm::InitializeAllTargetInfos();
		llvm::InitializeAllTargets();
		llvm::InitializeAllTargetMCs();
		llvm::InitializeAllAsmPrinters();
		llvm::InitializeAllAsmParsers();

		auto triple = llvm::sys::getDefaultTargetTriple();
		m->setTargetTriple(triple);

		//std::cout << "target triple = " << triple << std::endl;

		std::string err;
		auto target = llvm::TargetRegistry::lookupTarget(triple, err);

		std::string cpu = "generic";
		std::string features = "";
		llvm::TargetOptions opts;
		auto target_machine = target->createTargetMachine(triple, cpu, features, opts, {});

		m->setDataLayout(target_machine->createDataLayout());

		llvm::SmallString<256> output;
		llvm::raw_svector_ostream s_output(output);

		llvm::legacy::PassManager pm;

		auto ft = tgt == target::binary ? llvm::CGFT_ObjectFile : llvm::CGFT_AssemblyFile;

		if(target_machine->addPassesToEmitFile(pm, s_output, nullptr, ft)) {
			std::cerr << "ERROR: cannot emit file type" << std::endl;
		}
		pm.run(*m);

		return std::string(output);
	}
} // namespace etch
