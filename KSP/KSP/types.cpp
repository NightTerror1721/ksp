#include "types.h"

bool ksp::TypeInfo::_equals(const TypeInfo& type) const
{
	if (this == &type)
		return true;
	if (_kind != type._kind)
		return false;
	switch (_kind)
	{
		default: return true;
		case TypeKind::Pointer:
			return _componentType
				? type._componentType && _componentType->_equals(*type._componentType)
				: !type._componentType;
		case TypeKind::Array:
			return _elementCount == type._elementCount &&
				_componentType->_equals(*type._componentType);
	}
	return false;
}

bool operator== (const ksp::TypeInfo& t0, const ksp::TypeInfo& t1) { return t0._equals(t1); }
bool operator!= (const ksp::TypeInfo& t0, const ksp::TypeInfo& t1) { return !t0._equals(t1); }

bool operator== (const ksp::Type& t0, const ksp::Type& t1) { return t0._type->_equals(*t1._type); }
bool operator!= (const ksp::Type& t0, const ksp::Type& t1) { return !t0._type->_equals(*t1._type); }

bool operator== (const ksp::TypeInfo& t0, const ksp::Type& t1) { return t0._equals(*t1._type); }
bool operator!= (const ksp::TypeInfo& t0, const ksp::Type& t1) { return !t0._equals(*t1._type); }

bool operator== (const ksp::Type& t0, const ksp::TypeInfo& t1) { return t0._type->_equals(t1); }
bool operator!= (const ksp::Type& t0, const ksp::TypeInfo& t1) { return !t0._type->_equals(t1); }


void ksp::TypeInfo::_resetExtra()
{
	switch (_kind)
	{
		case TypeKind::Function:
			_parameters.~vector();
			break;
	}
}
void ksp::TypeInfo::_copyExtra(const TypeInfo& type)
{
	switch (_kind)
	{
		case TypeKind::Pointer:
			_componentType = type._componentType;
			break;
		case TypeKind::Array:
			_componentType = type._componentType;
			_elementCount = type._elementCount;
			break;
		case TypeKind::Function:
			_returnType = type._returnType;
			_parameters = type._parameters;
			break;
	}
}
void ksp::TypeInfo::_moveExtra(TypeInfo&& type) noexcept
{
	switch (_kind)
	{
	case TypeKind::Pointer:
		_componentType = std::move(type._componentType);
		break;
	case TypeKind::Array:
		_componentType = std::move(type._componentType);
		_elementCount = std::move(type._elementCount);
		break;
	case TypeKind::Function:
		_returnType = std::move(type._returnType);
		_parameters = std::move(type._parameters);
		break;
	}
}

ksp::TypeInfo::TypeInfo(const TypeKind kind, const size_t size) :
	_kind{ kind },
	_size{ size },
	_extra{}
{}
ksp::TypeInfo::TypeInfo(const TypeInfo& type) :
	TypeInfo{ type._kind, type._size }
{
	_copyExtra(type);
}
ksp::TypeInfo::TypeInfo(TypeInfo&& type) noexcept :
	_kind{ std::move(type._kind) },
	_size{ std::move(type._size) }
{
	_moveExtra(std::move(type));
}
ksp::TypeInfo::~TypeInfo()
{
	_resetExtra();
}

ksp::TypeInfo& ksp::TypeInfo::operator= (const TypeInfo& type)
{
	_resetExtra();
	_kind = type._kind;
	_size = type._size;
	_copyExtra(type);
	return *this;
}
ksp::TypeInfo& ksp::TypeInfo::operator= (TypeInfo&& type) noexcept
{
	_resetExtra();
	_kind = std::move(type._kind);
	_size = std::move(type._size);
	_moveExtra(std::move(type));
	return *this;
}



const ksp::TypeInfo ksp::TypeInfo::Invalid{};

const ksp::TypeInfo ksp::TypeInfo::Byte{ TypeKind::Byte, sizeof(int8_t) };
const ksp::TypeInfo ksp::TypeInfo::Short{ TypeKind::Short, sizeof(int16_t) };
const ksp::TypeInfo ksp::TypeInfo::Integer{ TypeKind::Integer, sizeof(int32_t) };
const ksp::TypeInfo ksp::TypeInfo::Long{ TypeKind::Long, sizeof(int64_t) };

const ksp::TypeInfo ksp::TypeInfo::UByte{ TypeKind::UByte, sizeof(uint8_t) };
const ksp::TypeInfo ksp::TypeInfo::UShort{ TypeKind::UShort, sizeof(uint16_t) };
const ksp::TypeInfo ksp::TypeInfo::UInteger{ TypeKind::UInteger, sizeof(uint32_t) };
const ksp::TypeInfo ksp::TypeInfo::ULong{ TypeKind::ULong, sizeof(uint64_t) };

const ksp::TypeInfo ksp::TypeInfo::Float{ TypeKind::Float, sizeof(float) };
const ksp::TypeInfo ksp::TypeInfo::Double{ TypeKind::Double, sizeof(double) };

const ksp::TypeInfo ksp::TypeInfo::Boolean{ TypeKind::Boolean, sizeof(bool) };

const ksp::TypeInfo ksp::TypeInfo::Character{ TypeKind::Character, sizeof(char16_t) };



ksp::TypeInfo ksp::TypeInfo::pointerOf(const TypeInfo& type)
{
	TypeInfo t{ TypeKind::Pointer, sizeof(void*) };
	t._componentType = &type;
	return std::move(t);
}

ksp::TypeInfo ksp::TypeInfo::arrayOf(const TypeInfo& type, const size_t element_count)
{
	TypeInfo t{ TypeKind::Array, type._size * element_count };
	t._componentType = &type;
	t._elementCount = element_count;
	return std::move(t);
}

ksp::TypeInfo ksp::TypeInfo::function(const TypeInfo& returnType, const std::vector<FunctionParameter>& parameters)
{
	TypeInfo t{ TypeKind::Function, sizeof(void*) };
	t._returnType = &returnType;
	INVOKE_CONSTRUCTOR(t._parameters, std::vector<FunctionParameter>, parameters);
	return std::move(t);
}



const ksp::Type ksp::Type::Invalid{ &ksp::TypeInfo::Invalid };

const ksp::Type ksp::Type::Byte{ &ksp::TypeInfo::Byte };
const ksp::Type ksp::Type::Short{ &ksp::TypeInfo::Short };
const ksp::Type ksp::Type::Integer{ &ksp::TypeInfo::Integer };
const ksp::Type ksp::Type::Long{ &ksp::TypeInfo::Long };

const ksp::Type ksp::Type::UByte{ &ksp::TypeInfo::UByte };
const ksp::Type ksp::Type::UShort{ &ksp::TypeInfo::UShort };
const ksp::Type ksp::Type::UInteger{ &ksp::TypeInfo::UInteger };
const ksp::Type ksp::Type::ULong{ &ksp::TypeInfo::ULong };

const ksp::Type ksp::Type::Float{ &ksp::TypeInfo::Float };
const ksp::Type ksp::Type::Double{ &ksp::TypeInfo::Double };

const ksp::Type ksp::Type::Boolean{ &ksp::TypeInfo::Boolean };

const ksp::Type ksp::Type::Character{ &ksp::TypeInfo::Character };
