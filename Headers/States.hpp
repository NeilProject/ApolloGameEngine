#pragma once

// C++
#include <iostream>
#include <map>
#include <string>

// Lua
#include <AP_Lua_CPP.hpp>

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

		std::string TraceBack();
		static std::string TraceBack(std::string state);
		static Apollo_State* Get(std::string state,std::string CalledByState="C++");

		static bool HasState(std::string state);

		~Apollo_State();
	};
}