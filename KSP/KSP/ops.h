#pragma once

#include "support.h"

namespace ksp
{
	namespace opcode
	{
		enum : opcode_t
		{
			NOP = 0x00, // ; pop[0]; push[0] // 

			LD_L, // <0..3 local_offset>; pop[0]; push[1] //

			ST8_L,  // <0..3 local_offset>; pop[1]; push[0] //
			ST16_L, // <0..3 local_offset>; pop[1]; push[0] //
			ST32_L, // <0..3 local_offset>; pop[1]; push[0] //
			ST64_L, // <0..3 local_offset>; pop[1]; push[0] //
		};
	}
}
