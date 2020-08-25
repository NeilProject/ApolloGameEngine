// C++
#include <iostream>

// Lua
#include <AP_Lua_CPP.hpp>

// Tricky's Units
#include <QuickString.hpp>

// Myself
#include "States.hpp"

namespace Tricky_Apollo {
	using namespace TrickyUnits;

	static std::map<std::string, Apollo_State> StateMap;

	void Apollo_State::SetName(std::string Name) {
		StateName = Name;
	}
	void Apollo_State::Init() {
		MyState = luaL_newstate();  /* create state */
		if (MyState == NULL) {
			std::cout << "\x1b[31mLua Error\x1b[0m " << "Cannot create state: not enough memory\n";
			Crash("Lua could not create a new state: Not enough memory", StateName, "", AE_LuaStateCreationFailure);
			return; // Now "Crash" should already end all stuff, but ya never know, so to make sure!
		}
	}
	
	void Apollo_State::Init(std::string State) {
		auto UState = Upper(State);
		if (StateMap.count(UState) > 0) {
			std::cout << "Destroying old state: " << StateMap[UState].StateName << "\n";
			StateMap.erase(UState);
		}
		std::cout << "Creating new state: " << State << "\n";
		StateMap[UState].StateName = State;
		StateMap[UState].Init();
	}

	std::string Apollo_State::TraceBack() {
		return "Traceback not a working feature yet";
	}

	std::string Apollo_State::TraceBack(std::string state) {
		if (state == "C++") return "Internal call";
		if (!HasState(state)) return "? TraceBack from non-existent state ?";
		return StateMap[Upper(state)].TraceBack();
	}

	Apollo_State* Apollo_State::Get(std::string state,std::string CalledByState) {
		auto us = Upper(state);
		if (StateMap.count(us) == 0) {
			Crash("State \"" + state + "\" does not exist!", CalledByState, TraceBack(CalledByState));
			return NULL;
		}
		return &(StateMap[us]);
	}

	bool Apollo_State::HasState(std::string state) {
		return StateMap.count(Upper(state)) > 0;
	}


	Apollo_State::~Apollo_State() {
		std::cout << "Destroy state " << StateName << "\n";
		lua_close(MyState);
	}
}