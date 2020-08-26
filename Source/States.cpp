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

	static std::vector< luaL_Reg > NeededFunctions;
	static std::vector< std::string > CoreLuaScripts;
	static std::vector< std::string > CoreNeilScripts;

	static int Apollo_Paniek(lua_State* L) {
		// Normally this should not happen, but just in case!
		// The "Lua Panic!" prefix is to make sure I know this happened.
		auto err = luaL_checkstring(L, 1);
		std::string Paniek = "Lua Panic!\n";
		Paniek += err;
		Crash(Paniek, "??", "??");
		return 0;
	}

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
		luaL_openlibs(MyState);
		lua_atpanic(MyState, Apollo_Paniek);
		// /* Test Lua
		luaL_loadstring(MyState, "print('Hello World! Testing new Lua State')");
		lua_call(MyState, 0, 0);
		// */		
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