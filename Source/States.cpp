// Lic:
// Apollo
// The United States of Lua
// 
// 
// 
// (c) Jeroen P. Broks, 2020, 2021
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
// Version: 21.03.09
// EndLic

// Interstate define debug
#define Interstate_define_debug

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
#include <Crash.hpp>

#undef Apollo_State_Debug

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
		std::string Trace = "";
		// /* DEBUG
		cout << lua_gettop(L) << "\n";
		for (int i = 1; i <= lua_gettop(L);i++) {
			cout << "Arg #" << i << "\t";
			switch (lua_type(L, i)) {
			case LUA_TSTRING:
				cout << "String \"" << luaL_checkstring(L, i);
				Trace += luaL_checkstring(L, i); Trace +="\n";
				break;
			case LUA_TNUMBER:
				cout << "Number " << luaL_checknumber(L, i);
			case LUA_TFUNCTION:
				cout << "Function";
			default:
				cout << "Unknown: " << lua_type(L, i);
				break;
			}
			cout << "\n";
		}
		// */
		// Normally this should not happen, but just in case!
		// The "Lua Panic!" prefix is to make sure I know this happened.
		//auto err = luaL_checkstring(L, 1);
		std::string Paniek = "Lua Panic!\n";
		//Paniek += err;
		Crash(Paniek, "C++: PANIC!", "Lua Dump:\n"+Trace);
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
				std::string tomod = "local source =\"" + TrickyUnits::bsdec(source) + "\"\n\nlocal func,err = load(source,\"" + scr.ReturnAs + "\")\n\nassert(func,\"Loading internal module '"+scr.Script+"' failed\\n\"..tostring(err))\n" + scr.ReturnAs + " = func()";
				// std::cout << "<huh>\n" << tomod << "\n</huh>\n\n"; continue;// HUH?
				luaL_loadstring(MyState, tomod.c_str());
				lua_call(MyState, 0, 0);
			} else {
#ifdef Apollo_State_Debug
				std::cout << "Adding to state: " << scr.Script << " as pure call!\n";
#endif
				luaL_loadstring(MyState, source.c_str());
				//std::cout << "<huh>\n" << source << "\n</huh>\n\n"; return;// HUH?
				lua_call(MyState, 0, 0);
			}
		}
		// State data in the state itself
		std::string StateData = "Group ApolloState\nreadonly string Name=\"" + StateName + "\"\nget string TraceBack\nreturn Lua.debug.traceback()\n end";
		StateData += "\n\nend";
		std::string exe = "local statedata = Neil.Load(\"" + bsdec(StateData) + "\")\nstatedata()";
		//exe += "\nprint('Apollo State:',Neil.Globals.ApolloState.Name) -- debug only\n"; // debug only
		luaL_loadstring(MyState, exe.c_str());
		lua_call(MyState, 0, 0);
		// And now for all the API linkup Neil Scripts
		std::cout << "Number of core Neil scripts: \x1b[34;1m" << CoreNeilScripts.size() << "\x1b[0m\n";
		for (auto& scr : CoreNeilScripts) {
			if (scr.isModule) {
				Crash("No support for Module for core Neil (yet)");
			} else {
#ifdef Apollo_State_Debug
				std::cout << "Executing core Neil Script: " << scr.Script << "\n";
#endif
				auto script = ARF.String(scr.Script);
				auto safe = bsdec(script);
				auto workout = "--[[CORECALL: "+scr.Script+"]]\nlocal success,lfunc,err = xpcall(Neil.Load,Apollo_Panic,\"" + safe + "\",\"ARF:"+scr.Script+"\")\n";
				//workout += "print ('DEBUG1: ',success,lfunc,err)\n"; // debug only!
				workout += "if not success then Apollo_Crash(lfunc,\"Neil Translation\") return end\n";
				workout += "if not lfunc then Apollo_Crash(err or 'Something really went wrong during the Neil translation') return end\n";
				//workout += "print ('DEBUG2: ',success,lfunc,err)\n"; // debug only!
				workout += "sucesss,err = xpcall(lfunc,Apollo_Panic)\n";
				//cout << "<NEIL>\n" << workout << "</NEIL>\n";
				luaL_loadstring(MyState, workout.c_str());
				lua_call(MyState, 0, 0);
			}
		}

	}
	void Apollo_State::Init() {
		if (MyState != NULL) lua_close(MyState);
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
				//std::cout << i << "> Registering " << n << " F:" << f << "\n"; // debug
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
		StateType = "";
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

	void Apollo_State::LoadString(std::string source, bool merge) {
		if (MyState == NULL || (!merge)) Init();
		//Crash("Loading a string into a Lua state has not yet been implemented","C++:Development","It's all in development folks!\nRome wasn't built in one day either, you know!\nSo please wait awhile longer!");
		//auto source = JCRPackage.String(script);
		std::string work = "--[[Apollo Load String]]\n";
		if (StateType == "Lua") {
			work += "local success,workfunc = xpcall(load,Apollo_Panic,\"" + bsdec(source) + "\",Neil.Globals.ApolloState.Name)\n";
		} else if (StateType=="Neil") {
			work += "local success,workfunc = xpcall(Neil.Load,Apollo_Panic,\"" + bsdec(source) + "\",Neil.Globals.ApolloState.Name)\n";
		} else {
			Crash("StateType has not been recognized: " + StateType);
		}
		work += "\n\nlocal e\n";
		work += "success,e=xpcall(workfunc,Apollo_Panic)\n";
		// cout << "<LOADED SCRIPT>\n" << work << "</LOADED SCRIPT>\n";
		luaL_loadstring(MyState,work.c_str());
		lua_call(MyState,0, 0);
	}

	void Apollo_State::LoadString(std::string state, std::string script, bool merge) {
		if (!merge) Init(state);
		if (!HasState(state)) Init(state);
		StateMap[Upper(state)].LoadString(script,true);
	}

	void Apollo_State::Load(std::string File, bool merge) {
		if (!merge) Init();
		if ((!merge) && suffixed(Upper(File), ".LUA")) StateType = "Lua";
		if ((!merge) && suffixed(Upper(File), ".NEIL")) StateType = "Neil";
		auto script = TrickyUnits::LoadString(File);
		LoadString(script,true);
	}

	void Apollo_State::Load(std::string State, std::string File, bool merge) {
		auto script = TrickyUnits::LoadString(File);
		if (!merge) Init(State);
		if ((!merge) && suffixed(Upper(File), ".LUA")) {
			auto S = Get(State); S->SetStateType("Lua");
		}
		if ((!merge) && suffixed(Upper(File), ".NEIL")) {
			auto S = Get(State); S->SetStateType("Neil");
		}
		LoadString(State,script,true);
	}

	void Apollo_State::Load(jcr6::JT_Dir& JD, std::string Entry, bool merge) {
		if (!merge) Init();
		if ((!merge) && suffixed(Upper(Entry), ".LUA")) StateType = "Lua";
		if ((!merge) && suffixed(Upper(Entry), ".NEIL")) StateType = "Neil";
		auto script = JD.String(Entry);
		LoadString(script,true);
	}

	void Apollo_State::Load(std::string State, jcr6::JT_Dir& JD, std::string Entry, bool merge) {
		auto script = JD.String(Entry);
		if (!merge) Init(State);
		if ((!merge) && suffixed(Upper(Entry), ".LUA")) {
			auto S = Get(State); S->SetStateType("Lua");
		}
		if ((!merge) && suffixed(Upper(Entry), ".NEIL")) {
			auto S = Get(State); S->SetStateType("Neil");
		}
		LoadString(State,script,true);

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

	void Apollo_State::SetStateType(std::string NewState) {
		StateType = NewState;
		cout << "State type '" << StateName << "' set to '" << StateType << "'\n";
	}

	bool Apollo_State::HasState(std::string state) {
		return StateMap.count(Upper(state)) > 0;
	}

	std::string Apollo_State::NameFromState(lua_State* L) {
		std::string ret = "Couldn't fetch state";
		luaL_loadstring(L, "return Neil.Globals.ApolloState.Name");
		lua_call(L,0, LUA_MULTRET,0);
		while (lua_gettop(L)) {
			if (lua_type(L,lua_gettop(L)) == LUA_TNIL) ret = "Couldn't fetch state";
			else if (lua_type(L, lua_gettop(L)) != LUA_TSTRING) Crash("NameFromState(<state>) failed!", "Lua link script"); return "ERROR";
			ret = lua_tostring(L, lua_gettop(L));
			lua_pop(L, 1);
		}
		return ret;
	}


	void Apollo_State::RequireFunction(const char* name, lua_CFunction func) {
		// cout << "DEBUG: RequireFunction(\""<<name<<"\",<cppfunction>);\n";
		NeededFunctions.push_back({ name,func });
	}

	void Apollo_State::RequireNeil(std::string name) {
		CoreNeilScripts.push_back({ false,"",name });
	}

	void Apollo_State::RequireLua(std::string name) {
		CoreLuaScripts.push_back({ false,"",name });
	}

	void Apollo_State::RawCall(std::string function, std::string parameters,int retvalues) {
	std:string work = "--[[RawCall]]\nif type(" + function + ")~='function' then\n\tApollo_Crash(\"Callback error:\\n" + function + " is not a function but a \"..type(" + function + "),  Neil.Globals.ApolloState.Name, Neil.Globals.ApolloState.TraceBack)\nelse\n\tlocal s,e=xpcall(" + function + ",Apollo_Panic," + parameters + ")\nreturn s\nend";
		//cout << "<RAWCALL>\n" << work << "\n</RAWCALL>\n";
		if (!MyState) {			
			Crash("RawCall to NULL state", "RawCall","RawCall(\""+function+"\",\""+parameters+"\","+std::to_string(retvalues)+"):\n\n<Work>\n"+work+"</work>");
			return;
		}
		luaL_loadstring(MyState, work.c_str());
		lua_call(MyState,0, 0,retvalues);
	}

	void Apollo_State::RawCall(std::string state, std::string function, std::string parameters,int retvalues) {
		Get(state)->RawCall(function, parameters, retvalues);
	}

	void Apollo_State::RawNeilCall(std::string function, std::string parameters, int retvalues ) {
		RawCall("Neil.Globals." + function, parameters,retvalues);
	}

	void Apollo_State::RawNeilCall(std::string state, std::string function, std::string parameters, int retvalues ) {
		RawCall(state,"Neil.Globals." + function, parameters,retvalues);
	}

	void Apollo_State::RawCallByType(std::string function, std::string parameters, int retvalues) {
		if (StateType == "Lua")
			RawCall(function, parameters,retvalues);
		else if (StateType == "Neil")
			RawNeilCall(function, parameters, retvalues);
		else
			Crash("Unknown State Type");
	}

	void Apollo_State::RawCallByType(std::string state, std::string function, std::string parameters, int retvalues) {
		Get(state)->RawCallByType(function, parameters,  retvalues);
	}

	void Apollo_State::Kill() {
		if (MyState) {
			lua_close(MyState);
			MyState = NULL;
		}
	}

	void Apollo_State::Kill(std::string state) {
		state = Upper(state);
		if (!StateMap.count(state)) {
			cout << "HUH! Kill request, non-existent state: " << state << endl;
			return;
		}
		StateMap[state].Kill();
		if (StateMap.count(state)) StateMap.erase(state);
	}

	void Apollo_State::KillAll() {
		vector<string> Moordenaar;
		for (auto Slachtoffer : StateMap) Moordenaar.push_back(Slachtoffer.first);
		for (auto Slachtoffer : Moordenaar) if (StateMap.count(Slachtoffer)) Kill(Slachtoffer);
	}

	int Apollo_State::top() {
		return lua_gettop(MyState);
	}

	int Apollo_State::ltype(int retnum) {
		return lua_type(MyState, retnum);
	}





	/*
	std::string Apollo_State::stype(int retnum) {
		switch (ltype(retnum)) {
		case LUA_TNIL:
			return "nil";
		case LUA_TBOOLEAN:
			return "boolean";
		case LUA_TFUNCTION:
			return "function";
		case LUA_TNUMBER:
			return "number";
		case LUA_TSTRING:
			return "string";
		case LUA_TLIGHTUSERDATA:
			return "userdata";
		default:
			return "unknown";
		}
	}

	int Apollo_State::GetInt(int retnum) {
		return floor(lua_tonumber(MyState,retnum));
	}

	string Apollo_State::GetStr(int retnum) {
		string ret = lua_tostring(MyState,retnum);
		return ret;
	}

	double Apollo_State::GetNum(int retnum) {
		return lua_tonumber(MyState,retnum);
	}

	bool Apollo_State::GetBool(int retnum) {
		return lua_toboolean(MyState,retnum);
	}
	*/

#define Fetch() \
	std::string work;\
	if (stauto && StateType == "Neil")\
		work = "return Neil.Globals." + call;\
	else\
		work = "return " + call;\
	luaL_loadstring(MyState, work.c_str());\
	lua_call(MyState, 0, 1, retvalues);


#define ISDefine(value)\
	std::string work{"--[[ISDefine]]\n"};\
	if (stauto && StateType == "Neil")\
		work += "Neil.Globals." + call;\
	else\
		work += call;\
	work += " = " + value;\
	luaL_loadstring(MyState, work.c_str());\
	lua_call(MyState, 0, 1, retvalues);
	//std::cout << "\n\n<Interstate_define_debug>\n"<<work<<"\n</Interstate_define_debug>\n\n";\

	
	std::string Apollo_State::FetchString(std::string call, bool stauto) {
		Fetch();
		return lua_tostring(MyState, -1);
	}

	int Apollo_State::FetchInt(std::string call, bool stauto) {
		Fetch();
		return lua_tointeger(MyState, -1);
	}

	double Apollo_State::FetchNumber(string call, bool stauto) {
		Fetch();
		return (double)lua_tonumber(MyState, -1);
	}

	bool Apollo_State::FetchBoolean(std::string call, bool stauto) {
		Fetch();
		return lua_toboolean(MyState, -1);
	}

	void Apollo_State::DefString(std::string call, std::string value, bool stauto) { ISDefine(string("\"") + value + "\""); }
	void Apollo_State::DefInt(std::string call, int value, bool stauto) { ISDefine(to_string(value)); }
	void Apollo_State::DefNumber(std::string call, double value, bool stauto) { ISDefine(to_string(value)); }

	void Apollo_State::DefBool(std::string call, bool value, bool stauto) {
		string v;
		if (value) v = "true"; else v = "false";
		ISDefine(v);
	}

	void Apollo_State::DefBool(std::string call, std::string value, bool stauto) { DefBool(call, Upper(value) == "TRUE", stauto); }

	Apollo_State::~Apollo_State() {
		std::cout << "Destroying state " << StateName << " -> " << MyState << "\n";
		if (MyState == NULL) {
			std::cout << " = Nothing to destroy! State was empty!\n";
			return;
		} else {/*
			if (StateType == "Lua") {
				std::string function = "Apollo_Dispose";
				std::string work = "--[[DisposeLua]]\nif not " + function + " then return end\n\nif type(" + function + ")~='function' then\n\tApollo_Crash(\"Callback error:\\n" + function + " is not a function but a \"..type(" + function + "),  Neil.Globals.ApolloState.Name, Neil.Globals.ApolloState.TraceBack)\nelse\n\tlocal s,e=xpcall(" + function + ",Apollo_Panic)\nend";
				//cout << "<RAWCALL>\n" << work << "\n</RAWCALL>\n";
				luaL_loadstring(MyState, work.c_str());
				lua_call(MyState, 0, 0);
			} else if (StateType == "Neil") {
				std::string function = "Apollo_Dispose";
				std::string work = "--[[DisposeNeil]]\nif not Neil.Globals.GlobalExists(\"" + function + "\") then return end\n\nif type(Neil.Globals." + function + ")~='function' then\n\tApollo_Crash(\"Callback error:\\n" + function + " is not a function but a \"..type(" + function + "),  Neil.Globals.ApolloState.Name, Neil.Globals.ApolloState.TraceBack)\nelse\n\tlocal s,e=xpcall(Neil.Globals." + function + ",Apollo_Panic)\nend";
				cout << "<huh>\n" << work << "\n</huh>\n\n"; // debug!
				luaL_loadstring(MyState, work.c_str());
				lua_call(MyState, 0, 0);
			}
			lua_close(MyState);
			MyState = NULL;
			*/
		}
	}


	static int LuaAPITest(lua_State* L) {
		std::cout << "A small step for a man\nbut a giant leap for mankind!\n";
		return 0;
	}

	static int LuaAPICrash(lua_State* L) {
		std::string errmessage = luaL_checkstring(L, 1);
		std::string state = "unknown"; if (!lua_isnil(L, 2)) state = luaL_checkstring(L, 2);
		std::string trace = ""; if (!lua_isnil(L, 3)) trace = luaL_checkstring(L, 3);
		Crash(errmessage, state, trace, AE_Script_Error);
		return 0;
	}

	void States_Init() {
		if (State_Init_Done) return;
		State_Init_Done = 0;
		NeededFunctions.push_back({ "Apollo_TestAPI_Function",LuaAPITest });
		NeededFunctions.push_back({ "Apollo_Crash",LuaAPICrash });
		CoreLuaScripts.push_back({ true,"Neil","Neil/Neil.lua" });
		CoreLuaScripts.push_back({ true,"Apollo_Panic","Neil/Panic.lua" });
		CoreLuaScripts.push_back({ false,"Apollo_Use","Neil/UseBindings.lua" });
	}
}