#pragma once

#include <cinttypes>
#include <string>
#include <vector>

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

#define __TPKIND_SCALAR_MASK ( \
	static_cast<int>(Typekind::Byte) | \
	static_cast<int>(Typekind::Short) | \
	static_cast<int>(Typekind::Integer) | \
	static_cast<int>(Typekind::Long) | \
	static_cast<int>(Typekind::UByte) | \
	static_cast<int>(Typekind::UShort) | \
	static_cast<int>(Typekind::UInteger) | \
	static_cast<int>(Typekind::ULong) | \
	static_cast<int>(Typekind::Boolean) | \
	static_cast<int>(Typekind::Character) \
)

	constexpr bool is_invalid(const Typekind kind) { return !static_cast<int>(kind); }

	constexpr bool is_scalar(const Typekind kind)
	{
		return (static_cast<int>(kind) & __TPKIND_SCALAR_MASK) != 0;
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


	class Type
	{
	private:
		Typekind _kind;
		size_t _size;

		union _Extra
		{
			// Pointer //
			struct
			{
				Type* _componentType;
			};

			// Array //
			struct
			{
				Type* _componentType;
				size_t _elementCount;
			};

			_Extra() = default;
			_Extra(const _Extra& ex)
			{
				_copy(ex);
			}
			_Extra(_Extra&& ex) noexcept
			{
				operator=(std::move(ex));
			}
			~_Extra() { destroy(); }

			_Extra& operator= (const _Extra& ex)
			{
				destroy();
				_copy(ex);
				return *this;
			}
			_Extra& operator= (_Extra&& ex) noexcept
			{
				_componentType = std::move(ex._componentType);
				ex._componentType = nullptr;
				return *this;
			}

			void destroy()
			{
				if (_componentType)
					delete _componentType;

				std::memset(this, 0, sizeof(_Extra));
			}

		private:
			void _copy(const _Extra& ex)
			{
				if (ex._componentType)
					_componentType = new Type{ ex._componentType };
				_elementCount = ex._elementCount;
			}
			
		} _extra;

		Type(const Typekind kind, const size_t size);

		bool _equals(const Type& type) const;

	public:
		Type(const Type& type);
		explicit inline Type(const Type* type) : Type{ *type } {}
		Type(Type&& type) noexcept;
		~Type();

		Type& operator= (const Type& type);
		Type& operator= (Type&& type) noexcept;

		inline Typekind kind() const { return _kind; }
		inline size_t size() const { return _size; }

		inline const Type* componentType() const { return _extra._componentType; }

		inline size_t arrayElementCount() const { return _extra._elementCount; }

		inline bool operator== (const Type& type) const { return _equals(type); }
		inline bool operator!= (const Type& type) const { return !_equals(type); }

		size_t arrayDimensions() const;

		Type pointer() const;

		Type array(const size_t element_count) const;

		static const Type Byte;
		static const Type Short;
		static const Type Integer;
		static const Type Long;
		static const Type UByte;
		static const Type UShort;
		static const Type UInteger;
		static const Type ULong;
		static const Type Boolean;
		static const Type Character;

		static Type invalid();

		static Type pointerOf(const Type& base, const size_t depth);
		static Type undefinedPointer();

		static Type arrayOf(const Type& base, const std::vector<size_t>& elementsForDimension);
	};


	/*namespace
	{
		class AbstractType;
		class InvalidType;
		class ScalarType;
		class PointerType;
		class ArrayType;
		class StructType;
	}

	namespace
	{
		class AbstractType
		{
		private:
			Typekind _kind;
			size_t _size;

			PointerType* _ptrForm;

			AbstractType(const Typekind kind, const size_t size);
			AbstractType(const AbstractType&) = delete;

			AbstractType& operator= (const AbstractType&) = delete;

		public:
			struct Field
			{
				const AbstractType& type;
				std::string identifier;
			};

			virtual ~AbstractType();

			inline bool isInvalid() const { return !static_cast<int>(_kind); }

			inline Typekind kind() const { return _kind; }
			inline size_t size() const { return _size; }

			const AbstractType& pointer();

			virtual const AbstractType& componentType() const;

			virtual size_t arrayElementCount() const;

			inline operator bool() const { return static_cast<int>(_kind); }
			inline bool operator! () const { return !static_cast<int>(_kind); }

			virtual bool operator== (const AbstractType& type) const = 0;
			virtual bool operator!= (const AbstractType& type) const = 0;

			friend class InvalidType;
			friend class ScalarType;
			friend class PointerType;
			friend class ArrayType;
			friend class StructType;

			static const AbstractType& Invalid;

			static const AbstractType& Byte;
			static const AbstractType& Short;
			static const AbstractType& Integer;
			static const AbstractType& Long;
			static const AbstractType& UByte;
			static const AbstractType& UShort;
			static const AbstractType& UInteger;
			static const AbstractType& ULong;
			static const AbstractType& Boolean;
			static const AbstractType& Character;

		};

		class InvalidType : public AbstractType
		{
		private:
			InvalidType();

			static const InvalidType Invalid;

		public:
			virtual ~InvalidType() = default;

			bool operator== (const AbstractType& type) const override;
			bool operator!= (const AbstractType& type) const override;

			friend class AbstractType;
		};

		class ScalarType : public AbstractType
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
			friend class AbstractType;

			virtual ~ScalarType() = default;

			bool operator== (const AbstractType& type) const override;
			bool operator!= (const AbstractType& type) const override;
		};


		class PointerType : public AbstractType
		{
		private:
			const AbstractType& _componentType;

			PointerType(const AbstractType& component_type);

		public:
			friend class AbstractType;

			virtual ~PointerType();

			const AbstractType& componentType() const override;

			bool operator== (const AbstractType& type) const override;
			bool operator!= (const AbstractType& type) const override;
		};


		class ArrayType : public AbstractType
		{
		private:
			const AbstractType& _componentType;
			const size_t _count;

			ArrayType(const AbstractType& component_type);

		public:
			friend class AbstractType;

			const AbstractType& componentType() const override;

			size_t arrayElementCount() const override;

			bool operator== (const AbstractType& type) const override;
			bool operator!= (const AbstractType& type) const override;
		};
	}


	class Type
	{
	private:
		AbstractType* _type;

		Type() : _type{ nullptr } {}

	public:
		inline Type(AbstractType* type) : _type{ type } {}
		inline Type(AbstractType& type) : _type{ &type } {}
		inline Type(const Type& type) : _type{ type._type }
	};*/
}
