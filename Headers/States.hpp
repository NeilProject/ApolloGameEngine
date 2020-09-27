// Lic:
// Apollo
// The United States of Lua
// 
// 
// 
// (c) Jeroen P. Broks, 2020
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// Please note that some references to data like pictures or audio, do not automatically
// fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 20.09.27
// EndLic
#pragma once

// C++
#include <iostream>
#include <map>
#include <string>

// Lua
#include <AP_Lua_CPP.hpp>

// JCR6
#include <jcr6_core.hpp>

// Apollo
#include <Crash.hpp>

namespace Tricky_Apollo {	
	class Apollo_State {
	private:
		lua_State* MyState = NULL;
		// static std::map<std::string, Apollo_State> StateMap; // C++ doesn't appear to like this getup, and since this is not to be meddled with anyway, let's go for the "dirty" method
		std::string StateName = "Nameless";
		void SetName(std::string Name);
		void InitScripts();
		std::string StateType = "Neil";
	public:
		/// <summary>
		/// Initiates the state, activates all libraries. Initiates Neil, and all that stuff.
		/// </summary>
		void Init();
		static void Init(std::string state);

		void LoadString(std::string script, bool merge = false);
		static void LoadString(std::string state, std::string script, bool merge = false);

		void Load(std::string File, bool merge = false);
		static void Load(std::string State, std::string File, bool merge = false);
		void Load(jcr6::JT_Dir& JD, std::string Entry, bool merge=false);
		static void Load(std::string State, jcr6::JT_Dir& JD, std::string Entry, bool merge = false);
		void LoadJCR6(std::string JCR6MainFile, std::string Entry, bool merge = false); // Couldn't be done in overload, due to parameter conflicts!
		void Load(std::string State, std::string JCR6MainFile, std::string Entry, bool merge = false);

		std::string TraceBack();
		static std::string TraceBack(std::string state);
		static Apollo_State* Get(std::string state,std::string CalledByState="C++");
		void SetStateType(std::string NewState);

		static bool HasState(std::string state);
		static std::string NameFromState(lua_State* L);

		/// <summary>
		/// When executed all new states will support this function as part of the API now.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="func"></param>
		static void RequireFunction(const char* name, lua_CFunction func);
		static void RequireNeil(std::string name);
		static void RequireLua(std::string name);

		/// <summary>
		/// Calls a function within this state
		/// </summary>
		/// <param name="function">Function name</param>
		/// <param name="parameters">Parameters in Lua syntax</param>
		void RawCall(std::string function, std::string parameters, int retvalues = 0);
		static void RawCall(std::string state, std::string function, std::string parameters,int retvalues=0);

		void RawNeilCall(std::string function, std::string parameters, int retvalues = 0);
		static void RawNeilCall(std::string state, std::string function, std::string parameters, int retvalues = 0);

		void RawCallByType(std::string function, std::string parameters, int retvalues = 0);
		static void RawCallByType(std::string state, std::string function, std::string parameters, int retvalues = 0);

		void Kill();
		static void Kill(std::string state);
		static void KillAll();

		int top();
		int ltype(int retnum);
		/*
		std::string stype(int retnum);
		int GetInt(int retnum);
		std::string  GetStr(int retnum);
		double GetNum(int retnum);
		bool GetBool(int retnum);
		*/

		std::string FetchString(std::string call, bool stauto = true);
		int FetchInt(std::string call, bool stauto = true);
		double FetchNumber(std::string call, bool stauto = true);
		bool FetchBoolean(std::string call, bool stauto = true);


		~Apollo_State();
	};

	/// <summary>
	/// Makes sure that he deep core stuff is added to states before getting to work
	/// </summary>
	void States_Init();
}