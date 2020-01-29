#pragma once

#include <cinttypes>
#include <string>
#include <vector>

#define INVOKE_CONSTRUCTOR(_Object, _Class, ...) new(&(_Object)) _Class{ __VA_ARGS__ }

namespace ksp
{
	typedef void* ptr_t;
	typedef char* stack_ptr_t;
	typedef char* data_ptr_t;
	typedef const char* const_data_ptr_t;
	typedef uint32_t reg_t;
	typedef reg_t* reg_ptr_t;

	typedef uint8_t opcode_t;
	typedef opcode_t* bytecode_t;

	typedef std::vector<char> data_block_t;


	template<typename _Ty>
	constexpr const _Ty& max(const _Ty& a, const _Ty& b)
	{
		return (a > b) ? (a) : (b);
	}
}
