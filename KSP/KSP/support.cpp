#include "support.h"

ksp::Type::Type(const Typekind kind, const size_t size) :
	_kind{ kind },
	_size{ size }
{}

const ksp::Type& ksp::Type::component_type() const { return Invalid; }


const ksp::Type& ksp::Type::Invalid = ksp::InvalidType::Invalid;

const ksp::Type& ksp::Type::Byte = ksp::ScalarType::Byte;
const ksp::Type& ksp::Type::Short = ksp::ScalarType::Short;
const ksp::Type& ksp::Type::Integer = ksp::ScalarType::Integer;
const ksp::Type& ksp::Type::Long = ksp::ScalarType::Long;
const ksp::Type& ksp::Type::UByte = ksp::ScalarType::UByte;
const ksp::Type& ksp::Type::UShort = ksp::ScalarType::UShort;
const ksp::Type& ksp::Type::UInteger = ksp::ScalarType::UInteger;
const ksp::Type& ksp::Type::ULong = ksp::ScalarType::ULong;
const ksp::Type& ksp::Type::Boolean = ksp::ScalarType::Boolean;
const ksp::Type& ksp::Type::Character = ksp::ScalarType::Character;



ksp::InvalidType::InvalidType() :
	Type{ static_cast<Typekind>(0), 0 }
{}

bool ksp::InvalidType::operator== (const Type& type) const { return !static_cast<int>(type._kind); }
bool ksp::InvalidType::operator!= (const Type& type) const { return static_cast<int>(type._kind); }

const ksp::InvalidType ksp::InvalidType::Invalid{};



ksp::ScalarType::ScalarType(const Typekind kind, const size_t size) :
	Type{ kind, size }
{}

bool ksp::ScalarType::operator== (const Type& type) const { return ksp::is_scalar(type._kind) && _kind == type._kind; }
bool ksp::ScalarType::operator!= (const Type& type) const { return !ksp::is_scalar(type._kind) || _kind != type._kind; }

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
