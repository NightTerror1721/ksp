#include "types.h"

bool ksp::TypeInfo::_equals(const TypeInfo& type) const
{

}

bool operator== (const ksp::TypeInfo& t0, const ksp::TypeInfo& t1) { return t0._equals(t1); }
bool operator!= (const ksp::TypeInfo& t0, const ksp::TypeInfo& t1) { return !t0._equals(t1); }

bool operator== (const ksp::Type& t0, const ksp::Type& t1) { return t0._type->_equals(*t1._type); }
bool operator!= (const ksp::Type& t0, const ksp::Type& t1) { return !t0._type->_equals(*t1._type); }

bool operator== (const ksp::TypeInfo& t0, const ksp::Type& t1) { return t0._equals(*t1._type); }
bool operator!= (const ksp::TypeInfo& t0, const ksp::Type& t1) { return !t0._equals(*t1._type); }

bool operator== (const ksp::Type& t0, const ksp::TypeInfo& t1) { return t0._type->_equals(t1); }
bool operator!= (const ksp::Type& t0, const ksp::TypeInfo& t1) { return !t0._type->_equals(t1); }


bool ksp::TypeInfo::_Extra::operator= (const _Extra& ex)
{
	_destroy();
	componentType = ex.componentType;
	elementCount = ex.elementCount;
}
bool ksp::TypeInfo::_Extra::operator= (_Extra&& ex) noexcept
{

}

void ksp::TypeInfo::_Extra::_destroy()
{
	componentType = nullptr;
	elementCount = 0;
}
