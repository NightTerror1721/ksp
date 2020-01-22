#include "runtime.h"

#include <cstdlib>

#include "vm.h"
#include "ops.h"

using ksp::CallInfo;
using ksp::ptr_t;
using ksp::stack_ptr_t;
using ksp::bytecode::RunnableBytecode;
using ksp::opcode_t;
using ksp::bytecode_t;

ksp::RuntimeState::RuntimeState(
	const size_t calls_stack_size,
	const size_t data_stack_size) :
	calls_base{ reinterpret_cast<CallInfo*>(std::malloc(calls_stack_size * sizeof(CallInfo))) },
	ci{ nullptr },
	calls_size{ calls_stack_size },
	data{ reinterpret_cast<stack_ptr_t>(std::malloc(data_stack_size * sizeof(stack_ptr_t))) },
	data_size{ data_stack_size },
	pc{},
	rret{}
{}
ksp::RuntimeState::~RuntimeState()
{
	std::free(calls_base);
	std::free(data);
}

void ksp::RuntimeState::push_call_info(const size_t locals, const size_t temps, const size_t var_args, const size_t heap_size)
{
	CallInfo* info;
	if (!ci)
	{
		info = calls_base;
		info->bottom = data;
		info->prev = nullptr;
	}
	else
	{
		info = ci + 1;
		info->bottom = ci->top;
		info->prev = ci;
	}
	
	info->varargs_base = reinterpret_cast<reg_ptr_t>(info->bottom);
	info->locals_base = info->varargs_base + var_args;
	info->temps_base = info->locals_base + locals;
	info->heap_base = reinterpret_cast<stack_ptr_t>(info->temps_base + temps);
	info->top = info->heap_base + heap_size;
	info->saved_pc = pc;
	ci = info;
}




#define STACK __stack
#define RET_REG STACK.rret
#define CI STACK.ci
#define PC STACK.pc
#define DECL_STACK RuntimeState STACK
#define STACK_PUSH_CALL_INFO(locals, temps, var_args, heap_size) STACK.push_call_info((locals), (temps), (var_args), (heap_size))

#define PC_SET(instruction) PC = (instruction)
#define PC_SHIFT(amount) PC += (amount)

#define GET_OPCODE() (*(PC))

#define vmswitch(op) switch(op)
#define vmcase(op) case ksp::opcode:: op :
#define vmbreak break

#define BYTE uint8_t
#define WORD uint16_t
#define LONG uint32_t
#define QUAD uint64_t
#define PTR ptr_t

#define __PC_GET_DATA(offset, type) (*reinterpret_cast<type*>((PC) + (offset)))
#define PC_GET_BYTE(offset) __PC_GET_DATA(offset, BYTE)
#define PC_GET_WORD(offset) __PC_GET_DATA(offset, WORD)
#define PC_GET_LONG(offset) __PC_GET_DATA(offset, LONG)
#define PC_GET_QUAD(offset) __PC_GET_DATA(offset, QUAD)

#define REG(section, offset) (CI->section[(offset)])
#define REG_PTR(section, offset) (CI->section + (offset))

#define VARARG_REG(offset, type) REG(varargs_base, offset)
#define LOCAL_REG(offset, type) REG(locals_base, offset)
#define TEMP_REG(offset, type) REG(temps_base, offset)

#define VARARG_REG_PTR(offset, type) REG_PTR(varargs_base, offset)
#define LOCAL_REG_PTR(offset, type) REG_PTR(locals_base, offset)
#define TEMP_REG_PTR(offset, type) REG_PTR(temps_base, offset)

#define GET_LOCAL_PTR(offset) (CI->locals_base + (offset))

#define __GET_LOCAL_DATA_PTR(offset, type) (reinterpret_cast<type*>(GET_LOCAL_PTR(offset)))
#define GET_LOCAL_BYTE_PTR(offset) __GET_LOCAL_DATA_PTR(offset, BYTE)
#define GET_LOCAL_WORD_PTR(offset) __GET_LOCAL_DATA_PTR(offset, WORD)
#define GET_LOCAL_DWORD_PTR(offset) __GET_LOCAL_DATA_PTR(offset, DWORD)
#define GET_LOCAL_QWORD_PTR(offset) __GET_LOCAL_DATA_PTR(offset, QWORD)

#define GET_LOCAL_BYTE(offset) (*__GET_LOCAL_DATA_PTR(offset, BYTE))
#define GET_LOCAL_WORD(offset) (*__GET_LOCAL_DATA_PTR(offset, WORD))
#define GET_LOCAL_DWORD(offset) (*__GET_LOCAL_DATA_PTR(offset, DWORD))
#define GET_LOCAL_QWORD(offset) (*__GET_LOCAL_DATA_PTR(offset, QWORD))

#define SET_LOCAL_BYTE(offset, value) GET_LOCAL_BYTE(offset) = (value)
#define SET_LOCAL_WORD(offset, value) GET_LOCAL_WORD(offset) = (value)
#define SET_LOCAL_DWORD(offset, value) GET_LOCAL_DWORD(offset) = (value)
#define SET_LOCAL_QWORD(offset, value) GET_LOCAL_QWORD(offset) = (value)

#define __READ_MEM(ptr, type) (*reinterpret_cast<type*>(ptr))
#define READ_BYTE_MEM(ptr) __READ_MEM(ptr, BYTE)
#define READ_WORD_MEM(ptr) __READ_MEM(ptr, WORD)
#define READ_DWORD_MEM(ptr) __READ_MEM(ptr, DWORD)
#define READ_QWORD_MEM(ptr) __READ_MEM(ptr, QWORD)

#define __WRITE_MEM(ptr, value, type) __READ_MEM(ptr, type) = (value)
#define WRITE_BYTE_MEM(ptr, value) __WRITE_MEM(ptr, value, BYTE)
#define WRITE_WORD_MEM(ptr, value) __WRITE_MEM(ptr, value, WORD)
#define WRITE_DWORD_MEM(ptr, value) __WRITE_MEM(ptr, value, DWORD)
#define WRITE_QWORD_MEM(ptr, value) __WRITE_MEM(ptr, value, QWORD)



void ksp::execute(KSP_State* ksp_state, KSP_Module* module, const RunnableBytecode& code)
{
	DECL_STACK;
	PC_SET(code.code);

	STACK_PUSH_CALL_INFO(0, 0, 0, 0);

	for (;; PC_SHIFT(1))
	{
		opcode_t op = GET_OPCODE();

		vmswitch(op)
		{
			vmcase(NOP) {
			} vmbreak;

			vmcase(LD_L) {

				STACK_PUSH(GET_LOCAL_PTR(PC_GET_DWORD(1)));

				PC_SHIFT(4);
			} vmbreak;

			vmcase(ST8_L) {

				SET_LOCAL_BYTE(PC_GET_DWORD(1), READ_BYTE_MEM(STACK_POP()));

				PC_SHIFT(4);
			} vmbreak;

			vmcase(ST16_L) {

				SET_LOCAL_WORD(PC_GET_DWORD(1), READ_WORD_MEM(STACK_POP()));

				PC_SHIFT(4);
			} vmbreak;

			vmcase(ST32_L) {

				SET_LOCAL_DWORD(PC_GET_DWORD(1), READ_DWORD_MEM(STACK_POP()));

				PC_SHIFT(4);
			} vmbreak;

			vmcase(ST64_L) {

				SET_LOCAL_QWORD(PC_GET_DWORD(1), READ_QWORD_MEM(STACK_POP()));

				PC_SHIFT(4);
			} vmbreak;
		}
	}
}
