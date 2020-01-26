#include "support.h"



ksp::Type::Type(const Typekind kind, const size_t size) :
	_kind{ kind },
	_size{ size },
	_extra{}
{}

ksp::Type::Type(const Type& type) :
	_kind{ type._kind },
	_size{ type._size },
	_extra{ type._extra }
{}

ksp::Type::Type(Type&& type) noexcept :
	_kind{ std::move(type._kind) },
	_size{ std::move(type._size) },
	_extra{ std::move(type._extra) }
{

}

ksp::Type::~Type()
{
	switch (_kind)
	{
		case Typekind::Pointer:
			if(_extra._componentType)
				delete _extra._componentType;
			break;
		case Typekind::Array:
			if (_extra._componentType)
				delete _extra._componentType;
			break;
	}
}

ksp::Type& ksp::Type::operator= (const Type& type)
{
	_kind = type._kind;
	_size = type._size;
	_extra = type._extra;
	return *this;
}

ksp::Type& ksp::Type::operator= (Type&& type) noexcept
{
	_kind = std::move(type._kind);
	_size = std::move(type._size);
	_extra = std::move(type._extra);
	return *this;
}

bool ksp::Type::_equals(const Type& type) const
{
	if (_kind == type._kind)
	{
		switch (_kind)
		{
			case Typekind::Pointer:
				return !_extra._componentType ? !type._extra._componentType :
					type._extra._componentType && _extra._componentType->_equals(*type._extra._componentType);

			case Typekind::Array:
				return _extra._elementCount == type._extra._elementCount &&
					_extra._componentType->_equals(*type._extra._componentType);

			default: return true;
		}
	}
	return false;
}

size_t ksp::Type::arrayDimensions() const
{
	if (_kind == Typekind::Array)
	{
		size_t dims = 0;
		for (const Type* t = _extra._componentType; t && t->_kind == Typekind::Array; t = t->_extra._componentType)
			++dims;
		return dims;
	}
	return 0;
}

ksp::Type ksp::Type::pointer() const
{
	Type t{ Typekind::Pointer, sizeof(void*) };
	t._extra._componentType = new Type{ this };
	return std::move(t);
}

ksp::Type ksp::Type::array(const size_t element_count) const
{
	Type t{ Typekind::Array, _size * element_count };
	t._extra._componentType = new Type{ this };
	t._extra._elementCount = element_count;
	return std::move(t);
}



const ksp::Type ksp::Type::Byte{ ksp::Typekind::Byte, sizeof(int8_t) };
const ksp::Type ksp::Type::Short{ ksp::Typekind::Short, sizeof(int16_t) };
const ksp::Type ksp::Type::Integer{ ksp::Typekind::Integer, sizeof(int32_t) };
const ksp::Type ksp::Type::Long{ ksp::Typekind::Long, sizeof(int64_t) };
const ksp::Type ksp::Type::UByte{ ksp::Typekind::UByte, sizeof(uint8_t) };
const ksp::Type ksp::Type::UShort{ ksp::Typekind::UShort, sizeof(uint16_t) };
const ksp::Type ksp::Type::UInteger{ ksp::Typekind::UInteger, sizeof(uint32_t) };
const ksp::Type ksp::Type::ULong{ ksp::Typekind::ULong, sizeof(uint64_t) };
const ksp::Type ksp::Type::Boolean{ ksp::Typekind::Boolean, sizeof(char) };
const ksp::Type ksp::Type::Character{ ksp::Typekind::Byte, sizeof(char16_t) };


ksp::Type ksp::Type::invalid() { return { static_cast<Typekind>(0), 0 }; }

ksp::Type ksp::Type::pointerOf(const Type& base, const size_t depth)
{
	if (base._kind != Typekind::Pointer && depth > 0)
	{
		Type t = base.pointer();
		for (size_t i = depth - 1; i > 0; --i)
			t = t.pointer();
		return std::move(t);
	}
	return invalid();
}
ksp::Type ksp::Type::undefinedPointer()
{
	return { ksp::Typekind::Pointer, sizeof(void*) };
}

ksp::Type ksp::Type::arrayOf(const Type& base, const std::vector<size_t>& elementsForDimension)
{
	if (base._kind != Typekind::Array && !elementsForDimension.empty())
	{
		Type t = base;
		const size_t* it = &elementsForDimension.back();
		const size_t* first = elementsForDimension.data();
		for (; it >= first; --it)
			t = t.array(*it);
		return std::move(t);
	}
	return invalid();
}


/*ksp::AbstractType::AbstractType(const Typekind kind, const size_t size) :
	_kind{ kind },
	_size{ size },
	_ptrForm{ nullptr }
{}

ksp::AbstractType::~AbstractType()
{
	if (_ptrForm)
		delete _ptrForm;
}

const ksp::AbstractType& ksp::AbstractType::pointer()
{
	if (!_ptrForm)
		_ptrForm = new PointerType(*this);
	return *_ptrForm;
}

const ksp::AbstractType& ksp::AbstractType::componentType() const { return Invalid; }

size_t ksp::AbstractType::arrayElementCount() const { return 0; }


const ksp::AbstractType& ksp::AbstractType::Invalid = ksp::InvalidType::Invalid;

const ksp::AbstractType& ksp::AbstractType::Byte = ksp::ScalarType::Byte;
const ksp::AbstractType& ksp::AbstractType::Short = ksp::ScalarType::Short;
const ksp::AbstractType& ksp::AbstractType::Integer = ksp::ScalarType::Integer;
const ksp::AbstractType& ksp::AbstractType::Long = ksp::ScalarType::Long;
const ksp::AbstractType& ksp::AbstractType::UByte = ksp::ScalarType::UByte;
const ksp::AbstractType& ksp::AbstractType::UShort = ksp::ScalarType::UShort;
const ksp::AbstractType& ksp::AbstractType::UInteger = ksp::ScalarType::UInteger;
const ksp::AbstractType& ksp::AbstractType::ULong = ksp::ScalarType::ULong;
const ksp::AbstractType& ksp::AbstractType::Boolean = ksp::ScalarType::Boolean;
const ksp::AbstractType& ksp::AbstractType::Character = ksp::ScalarType::Character;



ksp::InvalidType::InvalidType() :
	AbstractType{ static_cast<Typekind>(0), 0 }
{}

bool ksp::InvalidType::operator== (const AbstractType& type) const { return !static_cast<int>(type._kind); }
bool ksp::InvalidType::operator!= (const AbstractType& type) const { return static_cast<int>(type._kind); }

const ksp::InvalidType ksp::InvalidType::Invalid{};



ksp::ScalarType::ScalarType(const Typekind kind, const size_t size) :
	AbstractType{ kind, size }
{}

bool ksp::ScalarType::operator== (const AbstractType& type) const { return ksp::is_scalar(type._kind) && _kind == type._kind; }
bool ksp::ScalarType::operator!= (const AbstractType& type) const { return !ksp::is_scalar(type._kind) || _kind != type._kind; }

const ksp::ScalarType ksp::ScalarType::Byte{ ksp::Typekind::Byte, sizeof(int8_t) };
const ksp::ScalarType ksp::ScalarType::Short{ ksp::Typekind::Short, sizeof(int16_t) };
const ksp::ScalarType ksp::ScalarType::Integer{ ksp::Typekind::Integer, sizeof(int32_t) };
const ksp::ScalarType ksp::ScalarType::Long{ ksp::Typekind::Long, sizeof(int64_t) };
const ksp::ScalarType ksp::ScalarType::UByte{ ksp::Typekind::UByte, sizeof(uint8_t) };
const ksp::ScalarType ksp::ScalarType::UShort{ ksp::Typekind::UShort, sizeof(uint16_t) };
const ksp::ScalarType ksp::ScalarType::UInteger{ ksp::Typekind::UInteger, sizeof(uint32_t) };
const ksp::ScalarType ksp::ScalarType::ULong{ ksp::Typekind::ULong, sizeof(uint64_t) };
const ksp::ScalarType ksp::ScalarType::Boolean{ ksp::Typekind::Boolean, sizeof(char) };
const ksp::ScalarType ksp::ScalarType::Character{ ksp::Typekind::Byte, sizeof(char16_t) };



ksp::PointerType::PointerType(const AbstractType& component_type) :
	AbstractType{ ksp::Typekind::Pointer, sizeof(void*) },
	_componentType{ component_type }
{}

ksp::PointerType::~PointerType() {}

const ksp::AbstractType& ksp::PointerType::componentType() const { return _componentType; }

bool ksp::PointerType::operator== (const AbstractType& type) const { return ksp::is_pointer(type._kind) && _componentType == type.componentType(); }
bool ksp::PointerType::operator!= (const AbstractType& type) const { return !ksp::is_pointer(type._kind) || _componentType != type.componentType(); }*/
