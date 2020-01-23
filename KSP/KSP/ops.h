#pragma once

#include "support.h"

#include <string>
#include <vector>
#include <iostream>

namespace ksp
{
	struct OpcodeArgument
	{
		std::string name;
		uint8_t size;
	};

	class OpcodeInfo
	{
	private:
		opcode_t _code;
		std::string _name;
		std::vector<OpcodeArgument> _args;

	public:
		inline OpcodeInfo(const opcode_t code, const std::string& name, const std::vector<OpcodeArgument>& args) :
			_code{ code },
			_name{ name },
			_args{ args }
		{}
		inline ~OpcodeInfo() = default;

		inline opcode_t code() const { return _code; }

		inline const std::string& name() const { return _name; }

		inline size_t args_count() const { return _args.size(); }

		inline const OpcodeArgument& arg(const size_t index) const { return _args[index]; }
	};

	namespace opcode
	{
		enum : opcode_t
		{
			NOP = 0x00,

			PUTB,
			PUTW,
			PUTL,
			PUTQ,

			MOVB,
			MOVW,
			MOVL,
			MOVQ,
		};

#define __declop(_Opcode, ...) const OpcodeInfo _Opcode{ opcode::_Opcode, #_Opcode, { __VA_ARGS__ } }
		namespace info
		{
			__declop(NOP);

			__declop(PUTB, { "dst_reg", 1 }, { "byte_value", 1 });
			__declop(PUTW, { "dst_reg", 1 }, { "word_value", 2 });
			__declop(PUTL, { "dst_reg", 1 }, { "long_value", 4 });
			__declop(PUTQ, { "dst_reg", 1 }, { "quad_value", 8 });

			__declop(MOVB, { "src_reg", 1 }, { "dst_reg", 1 });
			__declop(MOVW, { "src_reg", 1 }, { "dst_reg", 1 });
			__declop(MOVL, { "src_reg", 1 }, { "dst_reg", 1 });
			__declop(MOVQ, { "src_reg", 1 }, { "dst_reg", 1 });
		}
#undef __declop
	}
}

std::ostream& operator<< (std::ostream& os, const ksp::OpcodeInfo& opinfo);
