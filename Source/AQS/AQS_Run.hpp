// Lic:
// Source/AQS/AQS_Run.hpp
// Apollo Quick Script (header)
// version: 22.04.30
// Copyright (C) 2022 Jeroen P. Broks
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// EndLic
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