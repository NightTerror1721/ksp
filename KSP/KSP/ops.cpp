#include "ops.h"

std::ostream& operator<< (std::ostream& os, const ksp::OpcodeInfo& opinfo)
{
	os << opinfo.name();
	int len = opinfo.args_count();
	for (int i = 0; i < len; ++i)
	{
		const auto& arg = opinfo.arg(i);
		os << " <" << arg.name << ": " << static_cast<int>(arg.size) << ">";
	}
	os << ";";
	return os;
}
