#include "runtime.h"

#include <cstdlib>
#include <iostream>

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

void ksp::RuntimeState::print_current_callinfo_registers() const
{
	if (ci)
	{
		size_t count = (reinterpret_cast<char*>(ci->heap_base) - reinterpret_cast<char*>(ci->regs_base)) / sizeof(reg_t);
		reg_ptr_t ptr = ci->regs_base;
		int idx = 0;
		while ((count--) > 0)
			std::cout << "reg" << (idx++) << " = " << *(ptr++) << std::endl;
	}
}

void ksp::RuntimeState::push_call_info(const uint8_t register_count, const size_t heap_size)
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
	
	info->regs_base = reinterpret_cast<reg_ptr_t>(info->bottom);
	info->heap_base = reinterpret_cast<stack_ptr_t>(info->regs_base + register_count);
	info->top = info->heap_base + heap_size;
	info->saved_pc = pc;
	ci = info;
}




#define STACK __stack
#define RET_REG STACK.rret
#define CI STACK.ci
#define PC STACK.pc
#define DECL_STACK RuntimeState STACK
#define STACK_PUSH_CALL_INFO(regs_count, heap_size) STACK.push_call_info((regs_count), (heap_size))

#define STACK_PRINT() STACK.print_current_callinfo_registers()

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

#define AS_QUAD(value) ((0xffffffffffffffffULL) & (value))

#define __PC_GET_DATA(offset, type) *reinterpret_cast<type*>(PC + (offset))
#define PC_GET_BYTE(offset) __PC_GET_DATA(offset, BYTE)
#define PC_GET_WORD(offset) __PC_GET_DATA(offset, WORD)
#define PC_GET_LONG(offset) __PC_GET_DATA(offset, LONG)
#define PC_GET_QUAD(offset) __PC_GET_DATA(offset, QUAD)

#define __REG(offset) (CI->regs_base[(offset)])
#define __REG_PTR(offset) (CI->regs_base + (offset))

#define __REG_GET(offset, type) (static_cast<type>(__REG(offset)))
#define __REG_SET(offset, value) (__REG(offset) = (value))

#define REG_GET_BYTE(offset) __REG_GET(offset, BYTE)
#define REG_GET_WORD(offset) __REG_GET(offset, WORD)
#define REG_GET_LONG(offset) __REG(offset)
#define REG_GET_QUAD(offset) (*reinterpret_cast<QUAD*>(__REG_PTR(offset)))

#define REG_SET_BYTE(offset, value) __REG_SET(offset, (value) & 0xffU)
#define REG_SET_WORD(offset, value) __REG_SET(offset, (value) & 0xffffU)
#define REG_SET_LONG(offset, value) __REG_SET(offset, value)
#define REG_SET_QUAD(offset, value) (*reinterpret_cast<QUAD*>(__REG_PTR(offset)) = (value))

#define REG_GET_PTR(offset) (*reinterpret_cast<PTR*>(__REG_PTR(offset)))
#define REG_SET_PTR(offset, value) (*reinterpret_cast<PTR*>(__REG_PTR(offset)) = (value))

#define __READ_MEM(ptr, type) (*reinterpret_cast<type*>(ptr))
#define READ_BYTE_MEM(ptr) __READ_MEM(ptr, BYTE)
#define READ_WORD_MEM(ptr) __READ_MEM(ptr, WORD)
#define READ_LONG_MEM(ptr) __READ_MEM(ptr, LONG)
#define READ_QUAD_MEM(ptr) __READ_MEM(ptr, QUAD)

#define __WRITE_MEM(ptr, value, type) __READ_MEM(ptr, type) = (value)
#define WRITE_BYTE_MEM(ptr, value) __WRITE_MEM(ptr, value, BYTE)
#define WRITE_WORD_MEM(ptr, value) __WRITE_MEM(ptr, value, WORD)
#define WRITE_LONG_MEM(ptr, value) __WRITE_MEM(ptr, value, LONG)
#define WRITE_QUAD_MEM(ptr, value) __WRITE_MEM(ptr, value, QUAD)



void ksp::execute(KSP_State* ksp_state, Module* module, const bytecode::RunnableBytecode& code)
{
	DECL_STACK;
	PC_SET(code.code);

	STACK_PUSH_CALL_INFO(2, 0);

	for (;; PC_SHIFT(1))
	{
		opcode_t op = GET_OPCODE();

		vmswitch(op)
		{
			vmcase(NOP) {
			} vmbreak;

			vmcase(PUTB) {

				REG_SET_BYTE(PC_GET_BYTE(1), PC_GET_BYTE(2));

				PC_SHIFT(2);
			} vmbreak;

			vmcase(PUTW) {

				REG_SET_WORD(PC_GET_BYTE(1), PC_GET_WORD(2));

				PC_SHIFT(3);
			} vmbreak;

			vmcase(PUTL) {

				REG_SET_LONG(PC_GET_BYTE(1), PC_GET_LONG(2));

				PC_SHIFT(5);
			} vmbreak;

			vmcase(PUTQ) {

				REG_SET_QUAD(PC_GET_BYTE(1), PC_GET_QUAD(2));

				PC_SHIFT(9);
			} vmbreak;

			vmcase(MOVB) {

				REG_SET_BYTE(PC_GET_BYTE(2), REG_GET_BYTE(PC_GET_BYTE(1)));

				PC_SHIFT(2);
			} vmbreak;

			vmcase(MOVW) {

				REG_SET_WORD(PC_GET_BYTE(1), REG_GET_WORD(PC_GET_BYTE(2)));

				PC_SHIFT(2);
			} vmbreak;

			vmcase(MOVL) {

				REG_SET_LONG(PC_GET_BYTE(1), REG_GET_LONG(PC_GET_BYTE(2)));

				PC_SHIFT(2);
			} vmbreak;

			vmcase(MOVQ) {

				REG_SET_QUAD(PC_GET_BYTE(1), REG_GET_QUAD(PC_GET_BYTE(2)));

				PC_SHIFT(2);
			} vmbreak;
		}

		STACK_PRINT();
	}
}
