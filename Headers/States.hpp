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
// Version: 20.08.26
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

		static bool HasState(std::string state);

		~Apollo_State();
	};
}