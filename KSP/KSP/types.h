#pragma once

#include <cinttypes>
#include <string>
#include <vector>

#include "support.h"

namespace ksp
{
	class TypeInfo;
	class Type;
}

bool operator== (const ksp::TypeInfo& t0, const ksp::TypeInfo& t1);
bool operator!= (const ksp::TypeInfo& t0, const ksp::TypeInfo& t1);

bool operator== (const ksp::Type& t0, const ksp::Type& t1);
bool operator!= (const ksp::Type& t0, const ksp::Type& t1);

bool operator== (const ksp::TypeInfo& t0, const ksp::Type& t1);
bool operator!= (const ksp::TypeInfo& t0, const ksp::Type& t1);

bool operator== (const ksp::Type& t0, const ksp::TypeInfo& t1);
bool operator!= (const ksp::Type& t0, const ksp::TypeInfo& t1);

namespace ksp
{
	typedef char type_identifier_t[32];

	enum class TypeKind
	{
		/* Invalid = 0x00, */
		//IntegralScalar = 0x01,
		//FloatingScalar,
		Byte,
		Short,
		Integer,
		Long,
		UByte,
		UShort,
		UInteger,
		ULong,
		Float,
		Double,
		Boolean,
		Character,
		Pointer,
		Array,
		Function
	};


	class Type;


	class TypeInfo
	{
	public: //Support structures
		struct FunctionParameter
		{
			const TypeInfo* type;
			std::string     name;
		};
		typedef FunctionParameter StructField;

	private:
		TypeKind _kind;
		size_t   _size;

		// Type specific part //
		union
		{
			// Pointer //
			struct
			{
				const TypeInfo* _componentType;
			};

			// Array //
			struct
			{
				const TypeInfo* _componentType;
				size_t          _elementCount;
			};

			// Function //
			struct
			{
				const TypeInfo* _returnType;
				std::vector<FunctionParameter> _parameters;
			};

			char _extra[
				max(
					max(
						sizeof(_componentType),
						sizeof(_componentType) + sizeof(_elementCount)),
					sizeof(_returnType) + sizeof(_parameters)
				)
			];
		};
		void _resetExtra();
		void _copyExtra(const TypeInfo& type);
		void _moveExtra(TypeInfo&& type) noexcept;

		TypeInfo(const TypeKind kind, const size_t size);

	public:
		inline TypeInfo() : TypeInfo{ static_cast<TypeKind>(0), 0 } {}
		TypeInfo(const TypeInfo& type);
		TypeInfo(TypeInfo&& type) noexcept;
		~TypeInfo();

		TypeInfo& operator= (const TypeInfo& type);
		TypeInfo& operator= (TypeInfo&& type) noexcept;

		inline TypeKind kind() const { return _kind; }
		inline size_t size() const { return _size; }

		inline bool isInvalid() const { return !static_cast<int>(_kind); }

		inline const TypeInfo* componentType() const { return _componentType; }

		inline size_t elementCount() const { return _elementCount; }

		inline const TypeInfo* returnType() const { return _returnType; }
		inline const std::vector<FunctionParameter>& parameters() const { return _parameters; }



	private:
		bool _equals(const TypeInfo& type) const;

	public:
		friend class Type;

		friend bool ::operator== (const ksp::TypeInfo& t0, const ksp::TypeInfo& t1);
		friend bool ::operator!= (const ksp::TypeInfo& t0, const ksp::TypeInfo& t1);
		friend bool ::operator== (const ksp::TypeInfo& t0, const ksp::Type& t1);
		friend bool ::operator!= (const ksp::TypeInfo& t0, const ksp::Type& t1);
		friend bool ::operator== (const ksp::Type& t0, const ksp::TypeInfo& t1);
		friend bool ::operator!= (const ksp::Type& t0, const ksp::TypeInfo& t1);
		friend bool ::operator== (const ksp::Type& t0, const ksp::Type& t1);
		friend bool ::operator!= (const ksp::Type& t0, const ksp::Type& t1);

	public:
		static const TypeInfo Invalid;

		static const TypeInfo Byte;
		static const TypeInfo Short;
		static const TypeInfo Integer;
		static const TypeInfo Long;

		static const TypeInfo UByte;
		static const TypeInfo UShort;
		static const TypeInfo UInteger;
		static const TypeInfo ULong;

		static const TypeInfo Float;
		static const TypeInfo Double;

		static const TypeInfo Boolean;

		static const TypeInfo Character;

	public:
		inline static TypeInfo invalid() { return std::move(TypeInfo{}); }

		static TypeInfo pointerOf(const TypeInfo& type);

		static TypeInfo arrayOf(const TypeInfo& type, const size_t element_count);

		static TypeInfo function(const TypeInfo& returnType, const std::vector<FunctionParameter>& parameters);
	};


	class Type
	{
	private:
		const TypeInfo* _type;

	public:
		inline Type() : _type{ nullptr } {}
		inline Type(const TypeInfo* const type) : _type{ type } {}
		inline Type(const TypeInfo& type) : _type{ &type } {}
		inline Type(const Type& type) : _type{ type._type } {}

		inline Type& operator= (const TypeInfo* const type)
		{
			_type = type;
			return *this;
		}
		inline Type& operator= (const Type& type) noexcept
		{
			_type = type._type;
			return *this;
		}

		inline const TypeInfo* operator& () const { return _type; }

		inline operator bool() const { return _type && !_type->isInvalid(); }
		inline bool operator! () const { return !_type || _type->isInvalid(); }

		inline operator const TypeInfo& () const { return *_type; }

		inline bool isInvalid() const { return !_type || _type->isInvalid(); }

		inline bool isByte() const { return _type->_kind == TypeKind::Byte; }
		inline bool isShort() const { return _type->_kind == TypeKind::Short; }
		inline bool isInteger() const { return _type->_kind == TypeKind::Integer; }
		inline bool isLong() const { return _type->_kind == TypeKind::Long; }
		inline bool isUByte() const { return _type->_kind == TypeKind::UByte; }
		inline bool isUShort() const { return _type->_kind == TypeKind::UShort; }
		inline bool isUInteger() const { return _type->_kind == TypeKind::UInteger; }
		inline bool isULong() const { return _type->_kind == TypeKind::ULong; }
		inline bool isFloat() const { return _type->_kind == TypeKind::Float; }
		inline bool isDouble() const { return _type->_kind == TypeKind::Double; }
		inline bool isBoolean() const { return _type->_kind == TypeKind::Boolean; }
		inline bool isCharacter() const { return _type->_kind == TypeKind::Character; }
		inline bool isPointer() const { return _type->_kind == TypeKind::Pointer; }
		inline bool isArray() const { return _type->_kind == TypeKind::Array; }

		inline TypeKind kind() const { return _type->_kind; }
		inline size_t size() const { return _type->_size; }

		inline Type componentType() { return _type->_componentType; }

		inline size_t elementCount() { return _type->_elementCount; }

		inline Type returnType() const { return _type->_returnType; }
		inline const std::vector<TypeInfo::FunctionParameter>& parameters() const { return _type->_parameters; }

	public:
		friend class TypeInfo;
		friend bool ::operator== (const ksp::Type& t0, const ksp::Type& t1);
		friend bool ::operator!= (const ksp::Type& t0, const ksp::Type& t1);
		friend bool ::operator== (const ksp::Type& t0, const ksp::TypeInfo& t1);
		friend bool ::operator!= (const ksp::Type& t0, const ksp::TypeInfo& t1);
		friend bool ::operator== (const ksp::TypeInfo& t0, const ksp::Type& t1);
		friend bool ::operator!= (const ksp::TypeInfo& t0, const ksp::Type& t1);
		friend bool ::operator== (const ksp::TypeInfo& t0, const ksp::TypeInfo& t1);
		friend bool ::operator!= (const ksp::TypeInfo& t0, const ksp::TypeInfo& t1);

	public:
		static const Type Invalid;

		static const Type Byte;
		static const Type Short;
		static const Type Integer;
		static const Type Long;

		static const Type UByte;
		static const Type UShort;
		static const Type UInteger;
		static const Type ULong;

		static const Type Float;
		static const Type Double;

		static const Type Boolean;

		static const Type Character;
	};
}
