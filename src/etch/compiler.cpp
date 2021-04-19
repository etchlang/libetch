#include <etch/analysis/semantics.hpp>
#include <etch/codegen.hpp>
#include <etch/compiler.hpp>
#include <etch/parser.hpp>
#include <etch/transform/fold.hpp>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>

namespace etch {
	std::string etch::compile(std::string_view sv, std::string name) {
		auto sm = parse(sv);
		auto am = analysis::semantics{}.run(sm);

		transform::fold{}.run(am);

		std::shared_ptr<llvm::LLVMContext> ctx = std::make_shared<llvm::LLVMContext>();
		std::shared_ptr<llvm::Module> m = std::make_shared<llvm::Module>(name, *ctx);

		codegen{ctx, m}.run(am);

		llvm::verifyModule(*m, &llvm::errs());

		// output LLVM assembly to string

		std::string ll;
		llvm::raw_string_ostream os(ll);
		os << *m;
		os.flush();

		// emit object

		llvm::InitializeAllTargetInfos();
		llvm::InitializeAllTargets();
		llvm::InitializeAllTargetMCs();
		llvm::InitializeAllAsmPrinters();
		llvm::InitializeAllAsmParsers();

		auto triple = llvm::sys::getDefaultTargetTriple();
		m->setTargetTriple(triple);

		std::cout << "target triple = " << triple << std::endl;

		std::string err;
		auto target = llvm::TargetRegistry::lookupTarget(triple, err);

		std::string cpu = "generic";
		std::string features = "";
		llvm::TargetOptions opts;
		auto target_machine = target->createTargetMachine(triple, cpu, features, opts, {});

		m->setDataLayout(target_machine->createDataLayout());

		llvm::SmallString<256> assembly;
		llvm::raw_svector_ostream s_asm(assembly);

		llvm::legacy::PassManager pm;
		auto ft = llvm::CGFT_AssemblyFile;

		if(target_machine->addPassesToEmitFile(pm, s_asm, nullptr, ft)) {
			std::cerr << "ERROR: cannot emit file type" << std::endl;
		}
		pm.run(*m);

		return std::string(assembly);
	}
} // namespace etch
