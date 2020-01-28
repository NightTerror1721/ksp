#pragma once

#include <cinttypes>
#include <string>

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

	enum class TypeKind : uint8_t
	{
		/* Invalid = 0x00, */
		IntegralScalar = 0x01,
		FloatingScalar,
		Pointer,
		Array
	};


	class Type;


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
				const TypeInfo* componentType;
			};

			// Array //
			struct
			{
				const TypeInfo* componentType;
				size_t          elementCount;
			};

			inline _Extra() { std::memset(this, 0, sizeof(_Extra)); }
			inline _Extra(const _Extra& ex) { operator=(ex); }
			inline _Extra(_Extra&& ex) noexcept { operator=(std::move(ex)); }
			inline ~_Extra() { _destroy(); }

			bool operator= (const _Extra& ex);
			bool operator= (_Extra&& ex) noexcept;

			void _destroy();

		} _extra;

		TypeInfo(const TypeKind kind, const size_t size);

	public:
		TypeInfo(const TypeInfo& type);
		TypeInfo(TypeInfo&& type) noexcept;
		~TypeInfo();

		bool operator= (const TypeInfo& type);
		bool operator= (TypeInfo&& type);

		inline TypeKind kind() const { return _kind; }
		inline size_t size() const { return _size; }

		inline Type componentType() { return _extra.componentType; }

		inline size_t elementCount() { return _extra.elementCount; }



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
	};


	class Type
	{
	private:
		const TypeInfo* _type;

	public:
		inline Type() : _type{ nullptr } {}
		inline Type(const TypeInfo* const type) : _type{ type } {}
		inline Type(const Type& type) : _type{ type._type } {}

		inline Type& operator= (const TypeInfo* const type)
		{
			_type = type;
			return *this;
		}
		inline Type& operator= (const Type& type)
		{
			_type = type._type;
			return *this;
		}

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
	};
}
