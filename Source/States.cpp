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

// Apollo
#include <QuickStream.hpp>
#include <Globals.hpp>

#define Apollo_State_Debug

namespace Tricky_Apollo {
	using namespace TrickyUnits;

	static std::map<std::string, Apollo_State> StateMap;

	typedef struct ScriptModule{
		bool isModule = false;
		std::string ReturnAs = "";
		std::string Script = "";
	} ScriptModule;

	static std::vector< luaL_Reg > NeededFunctions;
	static std::vector< ScriptModule > CoreLuaScripts;
	static std::vector< ScriptModule > CoreNeilScripts;
	static bool State_Init_Done = false;

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
	void Apollo_State::InitScripts() {
		// Lua first after that Neil (also because Neil itself is written in Lua, so Neil cannot be run without these)
		std::cout << "Number of core Lua scripts: \x1b[34;1m" << CoreLuaScripts.size() << "\x1b[0m\n";
		for (auto& scr : CoreLuaScripts) {
			auto source = ARF.String(scr.Script);
			if (scr.isModule) {
#ifdef Apollo_State_Debug
				std::cout << "Adding to state: " << scr.Script << " as module " << scr.ReturnAs << "!\n";
#endif
				std::string tomod = "local source =\"" + TrickyUnits::bsdec(source) + "\"\n\nlocal func = load(source,\"" + scr.ReturnAs + "\")\n\n" + scr.ReturnAs + " = func()";
				luaL_loadstring(MyState, tomod.c_str());
			} else {
#ifdef Apollo_State_Debug
				std::cout << "Adding to state: " << scr.Script << " as pure call!\n";
#endif
				luaL_loadstring(MyState, source.c_str());
			}
			lua_call(MyState, 0, 0);
		}
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
		if (NeededFunctions.size()) {
			std::cout << "There are \x1b[33m" << NeededFunctions.size() << "\x1b[0m API functions to be added (including NULL if that's added)\n";
			auto l = NeededFunctions[NeededFunctions.size() - 1];
			/*
			if (!(l.func == NULL && l.name == NULL)) {
				std::cout << "Adding the two NULL values needed to end it all\n";
				NeededFunctions.push_back({ NULL,NULL }); // C can't find the end otherwise.
			}
			//*/
			// /* 
			for (int i = 0; i < NeededFunctions.size(); ++i) {
				auto n = "Null";
				auto f = "Null";
				if (NeededFunctions[i].name) n = NeededFunctions[i].name;
				if (NeededFunctions[i].func) f = "Function present";
				std::cout << i << "> Registering " << n << " F:" << f << "\n";
				lua_register(MyState, NeededFunctions[i].name, NeededFunctions[i].func); // Ugly, but works?
			}
			// */
			
			//luaL_setfuncs(MyState, &NeededFunctions[0], 0);
			//luaL_newlib(MyState, &NeededFunctions[0]);


		} else {
			std::cout << "\x1b[41;1;37mWARNING\x1b[0m No API functions to be added yet!";
		}
		// /* Test Lua
		luaL_loadstring(MyState, "print('Hello World! Testing new Lua State')");
		luaL_loadstring(MyState, "Apollo_TestAPI_Function()"); // Testing API function
		lua_call(MyState, 0, 0);
		// */		
		InitScripts();
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
		Crash("Loading a string into a Lua state has not yet been implemented","C++:Development","It's all in development folks!\nRome wasn't built in one day either, you know!\nSo please wait awhile longer!");
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

	static int LuaAPITest(lua_State* L) {
		std::cout << "A small step for a man\nbut a giant leap for mankind!\n";
		return 0;
	}

	void States_Init() {
		if (State_Init_Done) return;
		State_Init_Done = 0;
		NeededFunctions.push_back({ "Apollo_TestAPI_Function",LuaAPITest });
		CoreLuaScripts.push_back({ true,"Neil","Neil/Neil.lua" });
	}
}