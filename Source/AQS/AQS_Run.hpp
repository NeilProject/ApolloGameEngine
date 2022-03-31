#pragma once

#include <memory>
#include <map>
#include <string>
#include <vector>

#include <jcr6_core.hpp>

namespace ApolloQuickScript {
	class True_AQS_Parameter;
	class True_AQS_Instruction;
	class True_AQS_Chunk;
	class True_AQS_State;

	typedef std::shared_ptr<True_AQS_Parameter> AQS_Parameter;
	typedef std::shared_ptr<True_AQS_Instruction> AQS_Instruction;
	typedef std::shared_ptr<True_AQS_Chunk> AQS_Chunk;
	typedef std::shared_ptr<True_AQS_State> AQS_State;

	typedef void(*AQS_LibFunc)(AQS_State s);

	class True_AQS_State {
	private:
		std::map<std::string, AQS_Chunk> Chunks;
		void _Load(jcr6::JT_Dir* d, std::string script);
		std::string scriptfile;
	public:
		static AQS_State Load(jcr6::JT_Dir* d, std::string script);
		void Register(std::string Chunk, AQS_LibFunc Func);
	};
	
}
