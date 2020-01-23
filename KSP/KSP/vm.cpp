#include "vm.h"


std::vector<ksp::opcode_t> ksp::bytecode::BytecodeBuilder::build() const
{
	return _codes;
}

void ksp::bytecode::BytecodeBuilder::push_opcode(const ksp::OpcodeInfo& op, const std::vector<opcode_t>& bytes)
{
	_codes.push_back(op.code());
	const int len = op.args_count();
	const int bytes_len = bytes.size();
	auto bytes_ptr = bytes.data();
	for (int i = 0; i < len; ++i)
		_codes.push_back(i < bytes_len ? *(bytes_ptr++) : 0);
}
