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
		class AttemptToModifyABuiltElement : std::exception {};

		class Buildable
		{
		private:
			bool _built;

		public:
			inline Buildable() : _built{ false } {};
			~Buildable() = default;

		protected:
			inline bool built()
			{
				if (!_built)
					throw AttemptToModifyABuiltElement{};
			}

			inline bool nonbuilt()
			{
				if (_built)
					throw AttemptToModifyABuiltElement{};
			}

			inline void build() { _built = true; }
		};

		class TypePool
		{
		private:
			std::map<std::string, TypeInfo> _types;

		public:
			TypePool();
			~TypePool();

			bool registerType(const std::string& name, const TypeInfo& type);
			bool registerType(const std::string& name, TypeInfo&& type);

			bool hasType(const std::string& name) const;

			Type getType(const std::string& name) const;

			std::string getTypeName(const Type& type) const;
		};


		class NamesTable
		{

		};



		/*struct GlobalValue
		{
			Type type;
			size_t offset{ 0 };
			data_ptr_t ptr{ nullptr };

			static const GlobalValue Invalid;
		};

		class ValuePool : private Buildable
		{
		private:
			std::map<std::string, GlobalValue> _values;
			data_block_t _data;

		public:
			ValuePool();
			~ValuePool();

			bool registerValue(const std::string& name, const Type& type);

			bool hasValueName(const std::string& name) const;

			const GlobalValue& getValue(const std::string& name) const;

			data_ptr_t getValueAccessor(const std::string& name);
			const_data_ptr_t getValueAccessor(const std::string& name) const;

			void build();

		public:
			data_ptr_t fastDataAccessor;
		};

		struct FunctionParameter
		{
			Type type;
			std::string name;
		};

		class Function : private Buildable
		{
		private:
			std::string _name;
			std::vector<FunctionParameter> _params;
			Type _retType;
			std::vector<data_ptr_t> _externs;
			data_block_t _code;

		public:
			Function(const std::string& name, const Type& ret_type, const std::vector<FunctionParameter>& params);
			~Function();

			const std::string& name() const;

			const std::vector<FunctionParameter>& parameters() const;

			const Type& returnType() const;

			void addExternElement(void* element_ptr);

			void build();

		public:
			data_ptr_t* fastExternAccessor;
			data_ptr_t  fastCodeAccessor;
		};*/
	}

	struct KSP_Module
	{
		module_info::TypePool types;
		//module_info::ValuePool constants;
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
