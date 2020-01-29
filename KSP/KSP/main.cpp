#include <iostream>

#include "runtime.h"
#include "vm.h"
#include "ops.h"
#include "types.h"

int main(int argc, char** argv)
{
	ksp::Type t = ksp::Type::Integer;
	std::cout << t.size() << std::endl;

	/*ksp::KSP_State state;
	ksp::KSP_Module mod;

	std::cout << ksp::opcode::info::MOVB << std::endl;

	ksp::bytecode::BytecodeBuilder builder;
	builder.push_opcode(ksp::opcode::info::PUTB, { 0, 128 });
	builder.push_opcode(ksp::opcode::info::MOVB, { 0, 1 });

	auto raw_codes = builder.build();

	ksp::bytecode::RunnableBytecode code;
	code.code = &raw_codes[0];
	code.size = raw_codes.size();

	ksp::execute(&state, &mod, code);*/

	return 0;
}
