#pragma once

#include "support.h"

#define __KSP_DEFAULT_DATA_STACK_SIZE (1024 * 1024)
#define __KSP_DEFAULT_CALLS_STACK_SIZE (1024)

namespace ksp
{
	typedef stack_ptr_t* temp_ptr_t;

	struct KSP_State;
	struct KSP_Module;

	namespace bytecode
	{
		struct RunnableBytecode;
	}


	struct CallInfo
	{
		stack_ptr_t top;
		stack_ptr_t heap_base;
		reg_ptr_t temps_base;
		reg_ptr_t locals_base;
		reg_ptr_t varargs_base;
		stack_ptr_t bottom;

		bytecode_t saved_pc;

		CallInfo* prev;

		CallInfo() = default;
		~CallInfo() = default;
	};

	struct RuntimeState
	{
		CallInfo* ci;
		CallInfo* calls_base;
		size_t calls_size;

		stack_ptr_t data;
		size_t data_size;

		reg_t rret;
		bytecode_t pc;

		RuntimeState(
			const size_t calls_stack_size = __KSP_DEFAULT_CALLS_STACK_SIZE,
			const size_t data_stack_size = __KSP_DEFAULT_DATA_STACK_SIZE);
		~RuntimeState();

		void push_call_info(const size_t locals, const size_t temps, const size_t var_args, const size_t heap_size);
	};



	void execute(KSP_State* kstate, KSP_Module* module, const bytecode::RunnableBytecode& code);
}
