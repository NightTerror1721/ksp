#include "vm.h"


ksp::module_info::TypePool::TypePool() :
	_types{}
{}
ksp::module_info::TypePool::~TypePool() {}

bool ksp::module_info::TypePool::registerType(const std::string& name, const TypeInfo& type)
{
	if (hasType(name))
		return false;
	_types[name] = type;
	return true;
}
bool ksp::module_info::TypePool::registerType(const std::string& name, TypeInfo&& type)
{
	if (hasType(name))
		return false;
	_types[name] = std::move(type);
	return true;
}

bool ksp::module_info::TypePool::hasType(const std::string& name) const
{
	return _types.find(name) != _types.end();
}

ksp::Type ksp::module_info::TypePool::getType(const std::string& name) const
{
	const auto& it = _types.find(name);
	return it != _types.end() ? it->second : ksp::TypeInfo::Invalid;
}

std::string ksp::module_info::TypePool::getTypeName(const Type& type) const
{
	for (const auto& pair : _types)
		if (pair.second == type)
			return pair.first;
	return "";
}



/*const ksp::module_info::GlobalValue ksp::module_info::GlobalValue::Invalid{};



ksp::module_info::ValuePool::ValuePool() :
	Buildable{},
	_values{},
	_data{},
	fastDataAccessor{ nullptr }
{}
ksp::module_info::ValuePool::~ValuePool() {}

bool ksp::module_info::ValuePool::registerValue(const std::string& name, const Type& type)
{
	nonbuilt();
	if (hasValueName(name))
		return false;
	_values[name] = std::move(GlobalValue{ type, _data.size(), nullptr });
	_data.resize(_data.size() + type.size(), static_cast<const char>(0));
	return true;
}

bool ksp::module_info::ValuePool::hasValueName(const std::string& name) const
{
	return _values.find(name) != _values.end();
}

const ksp::module_info::GlobalValue& ksp::module_info::ValuePool::getValue(const std::string& name) const
{
	const auto& it = _values.find(name);
	return it != _values.end() ? it->second : GlobalValue::Invalid;
}

ksp::data_ptr_t ksp::module_info::ValuePool::getValueAccessor(const std::string& name)
{
	const auto& it = _values.find(name);
	if (it != _values.end())
	{
		data_ptr_t ptr = it->second.ptr;
		return ptr ? ptr : &_data[it->second.offset];
	}
	return nullptr;
}
ksp::const_data_ptr_t ksp::module_info::ValuePool::getValueAccessor(const std::string& name) const
{
	const auto& it = _values.find(name);
	if (it != _values.end())
	{
		data_ptr_t ptr = it->second.ptr;
		return ptr ? ptr : &_data[it->second.offset];
	}
	return nullptr;
}

void ksp::module_info::ValuePool::build()
{
	nonbuilt();
	Buildable::build();
	if (_values.empty())
	{
		fastDataAccessor = nullptr;
	}
	else
	{
		fastDataAccessor = &_data[0];
		for (auto& pair : _values)
			pair.second.ptr = fastDataAccessor + pair.second.offset;
	}
}*/






std::vector<ksp::opcode_t> ksp::bytecode::BytecodeBuilder::build() const
{
	return _codes;
}

void ksp::bytecode::BytecodeBuilder::push_opcode(const ksp::OpcodeInfo& op, const std::vector<opcode_t>& bytes)
{
	_codes.push_back(op.code());
	const int len = op.args_count();
	const int bytes_len = bytes.size();
	auto bytes_ptr = bytes.data();
	for (int i = 0; i < len; ++i)
		_codes.push_back(i < bytes_len ? *(bytes_ptr++) : 0);
}
