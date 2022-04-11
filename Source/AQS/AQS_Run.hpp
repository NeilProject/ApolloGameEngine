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
	typedef unsigned char byte;

	typedef void(*AQS_LibFunc)(True_AQS_State* s, std::string CalledBy);
	typedef void(*AQS_TPanic)(std::string msg, std::string AQS_State, std::string Chunk, std::string CalledBy);

	void AQS_DefaultPanic(std::string msg, std::string AQS_State, std::string Chunk, std::string CalledBy);
	extern AQS_TPanic AQS_Panic;





	class True_AQS_State {
	private:
		std::map<std::string, AQS_Chunk> Chunks;
		AQS_Parameter LastCallParameters[256];
		int MaxPara{ -1 };
		void _Load(jcr6::JT_Dir* d, std::string script,std::string calledby="C++");
		std::string scriptfile;
		bool thereareerrors{ false };
		void param(AQS_Parameter p, std::string CB = "C++");

	public:
		static bool OverwriteProtection;
		static AQS_State Load(jcr6::JT_Dir* d, std::string script,std::string calledby="C++");
		bool HasChunk(std::string ch);
		void Register(std::string Chunk, AQS_LibFunc Func);
		int CheckInteger(byte idx,bool autoconvert=false);
		std::string CheckString(byte index, bool autoconvert = true);
		/// <summary>
		/// For those who are purist enough NOT wanting to use C++ std::string variables :-P
		/// </summary>
		/// <param name="str"></param>
		/// <param name="index"></param>
		/// <param name="autoconvert"></param>
		void CheckString(char* str, byte index, bool autoconvert = true);
		void Call(std::string Chunk,std::string calledby="C++");
		void ResetParams();
		void ParamInt(int value,std::string CB="C++");
		void ParamString(std::string value, std::string CB = "C++");		
	};
	
}
