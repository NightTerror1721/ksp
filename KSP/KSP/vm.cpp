#include "vm.h"


/* NAME TABLE */

ksp::module_info::NameTable::Element::Element() :
	_owner{ nullptr },
	_offset{ 0 },
	_kind{ Kind::Invalid },
	_extern{ false },
	_data{ nullptr }
{}
ksp::module_info::NameTable::Element::~Element()
{
	if (_extern)
	{
		switch (_kind)
		{
		case Kind::Type:
			delete reinterpret_cast<TypeInfo*>(_data);
			break;
		case Kind::Constant:
			delete reinterpret_cast<ConstantValue*>(_data);
			break;
		case Kind::Function:
			break;
		}
	}
}

ksp::Type ksp::module_info::NameTable::Element::createTypeInfo(const TypeInfo& type)
{
	_reset();
	_kind = Kind::Type;
	_extern = false;
	_data = new TypeInfo{ type };
	return const_cast<const TypeInfo*>(reinterpret_cast<TypeInfo*>(_data));
}
ksp::Type ksp::module_info::NameTable::Element::createTypeInfo(TypeInfo&& type)
{
	_reset();
	_kind = Kind::Type;
	_extern = false;
	_data = new TypeInfo{ std::move(type) };
	return const_cast<const TypeInfo*>(reinterpret_cast<TypeInfo*>(_data));
}

const ksp::module_info::ConstantValue* ksp::module_info::NameTable::Element::createConstantValue(const Type& type)
{
	_reset();
	_kind = Kind::Constant;
	_extern = false;
	_data = new ConstantValue{ type };
	return const_cast<const ConstantValue*>(reinterpret_cast<ConstantValue*>(_data));
}

void ksp::module_info::NameTable::Element::attachExternal(const Element& elem)
{
	_reset();
	_owner = elem._owner;
	_kind = elem._kind;
	_extern = true;
	_data = elem._data;
}


ksp::module_info::NameTable::NameTable() :
	_elems{},
	_refs{},
	fastDataAccessor{ nullptr }
{}
ksp::module_info::NameTable::~NameTable() {}

ksp::module_info::NameTable::Element& ksp::module_info::NameTable::createNewElement(const std::string& name)
{
	auto it = _elems.try_emplace(name, Element{});
	if(!it.second)
		throw ElementAlreadyExists{ name };
	it.first->second._owner = this;
	return it.first->second;
}

ksp::module_info::NameTable::Element& ksp::module_info::NameTable::getElement(const std::string& name)
{
	const auto& it = _elems.find(name);
	if (it == _elems.end())
		throw ElementNotFound{ name };
	return it->second;
}
const ksp::module_info::NameTable::Element& ksp::module_info::NameTable::getElement(const std::string& name) const
{
	const auto& it = _elems.find(name);
	if (it == _elems.end())
		throw ElementNotFound{ name };
	return it->second;
}

void ksp::module_info::NameTable::buildReferences()
{
	_refs.~vector();
	INVOKE_CONSTRUCTOR(_refs, std::vector<data_ptr_t>);
	_refs.reserve(_elems.size());

	size_t offCount = 0;
	for (auto& p : _elems)
	{
		auto& e = p.second;
		switch (e._kind)
		{
			case Kind::Constant:
				e._offset = offCount++;
				_refs.push_back(reinterpret_cast<ConstantValue*>(e._data)->_data);
				break;
		}
	}

	fastDataAccessor = offCount > 0 ? &_refs[0] : nullptr;
}



ksp::module_info::ConstantValue::ConstantValue(const Type& type) :
	_type{ type },
	_data{ new char[type.size()] }
{}
ksp::module_info::ConstantValue::~ConstantValue()
{
	delete[] _data;
}



ksp::module_info::Function::VariableInfo::VariableInfo(const Type& type, const std::string& name, const bool is_parameter) :
	_type{ type },
	_name{ name },
	_param{ is_parameter }
{}
ksp::module_info::Function::VariableInfo::~VariableInfo() {}


ksp::module_info::Function::Function() :
	_paramCount{ 0 },
	_returnType{},
	_vars{},
	_code{},
	fastRegisterCount{ 0 },
	fastParameterCount{ 0 },
	fastExtraStackSize{ 0 },
	fastCodeAccessor{ nullptr }
{}
ksp::module_info::Function::~Function() {}

void ksp::module_info::Function::setReturnType(const Type& type)
{
	_returnType = type;
}

void ksp::module_info::Function::_insertVar(const Type& type, const std::string& name, bool is_param)
{
	auto it = std::find_if(_vars.begin(), _vars.end(), [&name](const VariableInfo& p) { return p._name == name; });
	if (it != _vars.end())
		throw ParameterOrVariableAlreadyExists{ name };

	if (is_param && !_vars.empty())
	{
		it = std::find_if(_vars.begin(), _vars.end(), [](const VariableInfo& p) { return !p.isParameter(); });
		if (it == _vars.end())
			_vars.emplace_back(type, name, true);
		else _vars.emplace(it, type, name, true);
	}
	else _vars.emplace_back(type, name, is_param);
}

void ksp::module_info::Function::addOpcode(const opcode_t op)
{
	_code.push_back(op);
}
void ksp::module_info::Function::addOpcodes(const std::vector<opcode_t>& ops)
{
	_code.reserve(_code.size() + ops.size());
	_code.insert(_code.end(), ops.begin(), ops.end());
}

void ksp::module_info::Function::build()
{
	fastRegisterCount = static_cast<uint8_t>(_vars.size());
	fastParameterCount = _paramCount;
	fastCodeAccessor = _code.empty() ? nullptr : &_code[0];

	size_t extra = 0;
	for (const auto& v : _vars)
		extra += v._type.getExtraSizeRequired();
	fastExtraStackSize = extra;
}








/* MODULE */

ksp::Module::Module() :
	content{},
	fastFunctionAccessor{ nullptr },
	fastConstantAccessor{ nullptr }
{}
ksp::Module::~Module() {}

void ksp::Module::build()
{
	content.buildReferences();
}




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
