#pragma once

#include <cinttypes>
#include <string>

namespace ksp
{
	typedef void* ptr_t;
	typedef char* stack_ptr_t;
	typedef uint32_t reg_t;
	typedef reg_t* reg_ptr_t;

	typedef uint8_t opcode_t;
	typedef opcode_t* bytecode_t;

	enum class Typekind
	{
		Byte = 0x1,
		Short = 0x1 << 1,
		Integer = 0x1 << 2,
		Long = 0x1 << 3,
		UByte = 0x1 << 4,
		UShort = 0x1 << 5,
		UInteger = 0x1 << 6,
		ULong = 0x1 << 7,
		Boolean = 0x1 << 8,
		Character = 0x1 << 9,
		Pointer = 0x1 << 10,
		Array = 0x1 << 11,
		Struct = 0x1 << 12
	};

	constexpr bool is_invalid(const Typekind kind) { return !static_cast<int>(kind); }

	constexpr bool is_scalar(const Typekind kind)
	{
		return (static_cast<int>(kind) & (
			static_cast<int>(Typekind::Byte) |
			static_cast<int>(Typekind::Short) |
			static_cast<int>(Typekind::Integer) |
			static_cast<int>(Typekind::Long) |
			static_cast<int>(Typekind::UByte) |
			static_cast<int>(Typekind::UShort) |
			static_cast<int>(Typekind::UInteger) |
			static_cast<int>(Typekind::ULong) |
			static_cast<int>(Typekind::Boolean) |
			static_cast<int>(Typekind::Character)
		)) != 0;
	}

	constexpr bool is_pointer(const Typekind kind)
	{
		return (static_cast<int>(kind) & (static_cast<int>(Typekind::Pointer))) != 0;
	}

	constexpr bool is_array(const Typekind kind)
	{
		return (static_cast<int>(kind) & (static_cast<int>(Typekind::Array))) != 0;
	}

	constexpr bool is_struct(const Typekind kind)
	{
		return (static_cast<int>(kind) & (static_cast<int>(Typekind::Struct))) != 0;
	}

	namespace
	{
		class InvalidType;
		class ScalarType;
		class PointerType;
		class ArrayType;
		class StructType;
	}

	class Type
	{
	private:
		Typekind _kind;
		size_t _size;

		PointerType* _ptrForm;

		Type(const Typekind kind, const size_t size);
		Type(const Type&) = delete;

		Type& operator= (const Type&) = delete;

	public:
		struct Field
		{
			const Type& type;
			std::string identifier;
		};

		virtual ~Type();

		inline bool isInvalid() const { return !static_cast<int>(_kind); }

		inline Typekind kind() const { return _kind; }
		inline size_t size() const { return _size; }

		const Type& pointer();

		virtual const Type& componentType() const;

		inline operator bool() const { return static_cast<int>(_kind); }
		inline bool operator! () const { return !static_cast<int>(_kind); }

		virtual bool operator== (const Type& type) const = 0;
		virtual bool operator!= (const Type& type) const = 0;

		friend class InvalidType;
		friend class ScalarType;
		friend class PointerType;
		friend class ArrayType;
		friend class StructType;

		static const Type& Invalid;

		static const Type& Byte;
		static const Type& Short;
		static const Type& Integer;
		static const Type& Long;
		static const Type& UByte;
		static const Type& UShort;
		static const Type& UInteger;
		static const Type& ULong;
		static const Type& Boolean;
		static const Type& Character;

	};

	namespace
	{
		class InvalidType : public Type
		{
		private:
			InvalidType();

			static const InvalidType Invalid;

		public:
			virtual ~InvalidType() = default;

			bool operator== (const Type& type) const override;
			bool operator!= (const Type& type) const override;

			friend class Type;
		};

		class ScalarType : public Type
		{
		private:
			ScalarType(const Typekind kind, const size_t size);

			static const ScalarType Byte;
			static const ScalarType Short;
			static const ScalarType Integer;
			static const ScalarType Long;
			static const ScalarType UByte;
			static const ScalarType UShort;
			static const ScalarType UInteger;
			static const ScalarType ULong;
			static const ScalarType Boolean;
			static const ScalarType Character;

		public:
			friend class Type;

			virtual ~ScalarType() = default;

			bool operator== (const Type& type) const override;
			bool operator!= (const Type& type) const override;
		};


		class PointerType : public Type
		{
		private:
			const Type& _componentType;

			PointerType(const Type& component_type);

		public:
			friend class Type;

			virtual ~PointerType();

			const Type& componentType() const override;

			bool operator== (const Type& type) const override;
			bool operator!= (const Type& type) const override;
		};


		class ArrayType : public Type
		{
		private:
			const Type& _componentType;
			const size_t _count;

			ArrayType(const Type& component_type);

		public:
			friend class Type;

			const Type& componentType() const override;

			bool operator== (const Type& type) const override;
			bool operator!= (const Type& type) const override;
		};
	} 
}
