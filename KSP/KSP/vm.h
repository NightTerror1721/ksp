#pragma once

#include <vector>

#include "support.h"
#include "ops.h"

namespace ksp
{
	namespace module_info
	{
		struct ConstantPool
		{
			size_t count;
			ptr_t data;
		};
	}

	struct KSP_Module
	{

	};

	struct KSP_State
	{

	};

	namespace bytecode
	{
		struct RunnableBytecode
		{
			bytecode_t code;
			size_t     size;
		};

		class BytecodeBuilder
		{
		private:
			std::vector<opcode_t> _codes;

		public:
			BytecodeBuilder() = default;
			~BytecodeBuilder() = default;

			std::vector<opcode_t> build() const;

			void push_opcode(const ksp::OpcodeInfo& op, const std::vector<opcode_t>& bytes);
		};
	}
}
