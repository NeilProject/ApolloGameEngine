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
// C++
#include <iostream>

// Lua
#include <AP_Lua_CPP.hpp>

// Tricky's Units
#include <QuickString.hpp>

// Myself
#include <States.hpp>
#include <QuickStream.hpp>

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

	void Apollo_State::LoadString(std::string script, bool merge) {
		if (MyState == NULL) Init();
		Crash("Loading a string into a Lua state has not yet been implemented","C++:Development","It's all in development folks!\nRome wasn't built in one day either, you know!\nSo please wait ahile longer!");
	}

	void Apollo_State::LoadString(std::string state, std::string script, bool merge) {
		if (!merge) Init(state);
		if (!HasState(state)) Init(state);
		StateMap[Upper(state)].LoadString(script);
	}

	void Apollo_State::Load(std::string File, bool merge) {
		auto script = TrickyUnits::LoadString(File);
		LoadString(script, merge);
	}

	void Apollo_State::Load(std::string State, std::string File, bool merge) {
		auto script = TrickyUnits::LoadString(File);
		LoadString(State,script, merge);
	}

	void Apollo_State::Load(jcr6::JT_Dir& JD, std::string Entry, bool merge) {
		auto script = JD.String(Entry);
		LoadString(script, merge);
	}

	void Apollo_State::Load(std::string State, jcr6::JT_Dir& JD, std::string Entry, bool merge) {
		auto script = JD.String(Entry);
		LoadString(State,script, merge);
	}

	void Apollo_State::LoadJCR6(std::string JCR6MainFile, std::string Entry, bool merge){
		auto J = jcr6::Dir(JCR6MainFile);
		Load(J, Entry, merge);
	}

	void Apollo_State::Load(std::string State, std::string JCR6MainFile, std::string Entry, bool merge) {
		auto J = jcr6::Dir(JCR6MainFile);
		Load(State,J, Entry, merge);
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
		if (MyState == NULL)
			std::cout << " = Nothing to destroy! State was empty!\n";
		else
			lua_close(MyState);
	}
}