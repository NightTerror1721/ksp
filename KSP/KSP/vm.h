#pragma once

#include <exception>
#include <vector>
#include <map>

#include "support.h"
#include "ops.h"
#include "types.h"

namespace ksp
{
	namespace module_info
	{
		class ConstantValue;

		class NameTable
		{
		public:
			enum class Kind
			{
				Invalid,
				Type,
				Constant,
				Function
			};

			class ElementAlreadyExists : std::exception
			{
			public:
				inline ElementAlreadyExists(const std::string& name) :
					exception{ ("Trying to create a new element with an already exists name: " + name).c_str() }
				{}
				inline ElementAlreadyExists(const char* name) :
					ElementAlreadyExists{ std::string{ name } }
				{}
			};

			class ElementNotFound : std::exception
			{
			public:
				inline ElementNotFound(const std::string& name) :
					exception{ ("Trying to get a element with name: " + name + ". But not found.").c_str() }
				{}
				inline ElementNotFound(const char* name) :
					ElementNotFound{ std::string{ name } }
				{}
			};

			class Element
			{
			private:
				NameTable* _owner;
				size_t _offset;
				Kind _kind;
				bool _extern;
				void* _data;

			public:
				Element();
				~Element();

				inline const NameTable& owner() const { return *_owner; }
				inline Kind kind() const { return _kind; }
				inline bool isExtern() const { return _extern; }

				inline Type getTypeMeta() const { return reinterpret_cast<TypeInfo*>(_data); }

				Type createTypeInfo(const TypeInfo& type);
				Type createTypeInfo(TypeInfo&& type);

				const ConstantValue* createConstantValue(const Type& type);

				//Function& createFunction()

				void attachExternal(const Element& elem);

			private:
				inline void _reset() { Element::~Element(); }

				friend class NameTable;
			};

		private:
			std::map<std::string, Element> _elems;
			std::vector<data_ptr_t> _refs;

		public:
			NameTable();
			~NameTable();

			Element& createNewElement(const std::string& name);

			Element& getElement(const std::string& name);
			const Element& getElement(const std::string& name) const;

			void buildReferences();

			inline bool hasName(const std::string& name) const { return _elems.find(name) != _elems.end(); }

		public:
			data_ptr_t* fastDataAccessor;
		};

		class ConstantValue
		{
		private:
			const Type		 _type;
			data_ptr_t const _data;

		public:
			ConstantValue(const Type& type);
			~ConstantValue();

			inline Type type() const { return _type; }
			inline data_ptr_t data() const { return _data; }

			friend class NameTable;
		};



		class Function
		{
		public:
			class VariableInfo
			{
			private:
				Type		_type;
				std::string _name;
				bool		_param;

			public:
				VariableInfo() = default;
				VariableInfo(const Type& type, const std::string& name, const bool is_parameter);
				~VariableInfo();

				inline Type type() const { return _type; }
				inline const std::string& name() const { return _name; }
				inline bool isParameter() const { return _param; }

				friend class Function;
			};

			class ParameterOrVariableAlreadyExists : std::exception
			{
			public:
				inline ParameterOrVariableAlreadyExists(const std::string& name) :
					exception{ ("Trying to create a new Parameter with an already exists name: " + name).c_str() }
				{}
				inline ParameterOrVariableAlreadyExists(const char* name) :
					ParameterOrVariableAlreadyExists{ std::string{ name } }
				{}
			};

		private:
			uint8_t _paramCount;
			Type _returnType;
			std::vector<VariableInfo> _vars;
			std::vector<opcode_t> _code;

		public:
			Function();
			~Function();

			void setReturnType(const Type& type);

			void addOpcode(const opcode_t op);
			void addOpcodes(const std::vector<opcode_t>& ops);

			void build();


			inline void addVariable(const Type& type, const std::string& name) { _insertVar(type, name, false); }
			inline void addParameter(const Type& type, const std::string& name) { _insertVar(type, name, true); }

			inline Type returnType() const { _returnType; }

			inline size_t parameterCount() const { return _paramCount; }
			inline const VariableInfo& parameter(const size_t index) const { return _vars[index]; }
			inline const std::vector<VariableInfo>& parameters() const { return _vars; }

			inline uint8_t variableCount() const { return static_cast<uint8_t>(_vars.size()); }
			inline const VariableInfo& variable(const size_t index) const { return _vars[index]; }
			inline const std::vector<VariableInfo>& variables() const { return _vars; }

			inline size_t opcodeCount() const { return _code.size(); }
			inline opcode_t opcode(const size_t index) const { return _code[index]; }
			inline const std::vector<opcode_t>& opcodes() const { return _code; }

		public:
			uint8_t fastRegisterCount;
			uint8_t fastParameterCount;
			bytecode_t fastCodeAccessor;
			size_t fastExtraStackSize;

		private:
			void _insertVar(const Type& type, const std::string& name, bool is_param);
		};
	}

	struct Module
	{
		module_info::NameTable content;

		// Fast Accessors //
		module_info::Function* fastFunctionAccessor;
		data_ptr_t* fastConstantAccessor;

		Module();
		~Module();

		void build();
	};

	struct KSP_State
	{

	};

	namespace bytecode
	{
		struct RunnableBytecode
		{
			bytecode_t code;
			size_t     size;
		};

		class BytecodeBuilder
		{
		private:
			std::vector<opcode_t> _codes;

		public:
			BytecodeBuilder() = default;
			~BytecodeBuilder() = default;

			std::vector<opcode_t> build() const;

			void push_opcode(const ksp::OpcodeInfo& op, const std::vector<opcode_t>& bytes);
		};
	}
}
