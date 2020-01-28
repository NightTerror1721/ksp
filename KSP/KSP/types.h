#pragma once

#include <cinttypes>
#include <string>

namespace ksp
{
	typedef char type_identifier_t[32];

	enum class TypeKind : uint8_t
	{
		/* Invalid = 0x00, */
		IntegralScalar = 0x01,
		FloatingScalar,
		Pointer,
		Array
	};

	/*class TypeInfo;

	class Namespace
	{
	private:
		struct Node
		{
			std::string name;
			Node* next;
			Node* prev;
		};
	};

	class Type
	{
	private:
		const TypeInfo* _info;
	};

	class TypeInfo
	{
	private:
		TypeKind _kind;
		size_t   _size;

		// Type specific part //
		union _Extra
		{
			// Pointer //
			struct
			{
				
			};

			// Array //
		} _extra;
	};*/
}
