#include <AP_Lua_CPP.hpp>
#include <States.hpp>
#include <Globals.hpp>
#include <QuickString.hpp>
#include "../AQS/AQS_Run.hpp"

using namespace std;
using namespace TrickyUnits;
using namespace ApolloQuickScript;

namespace Tricky_Apollo {

	map <string, AQS_State> AStates{};

	static void Paniek(std::string msg, std::string AQS_State, std::string Chunk, std::string CalledBy) {
		string TraceBack{ "No Neil/Lua traceback available" };
		if (Apollo_State::HasState(CalledBy)) TraceBack = Apollo_State::TraceBack(CalledBy);
		Crash("AQS Error: " + msg + "\nAQS(" + AQS_State + "); Chunk: " + Chunk, CalledBy, TraceBack, 400);
	}

	static int AQSA_Load(lua_State* L) {
		auto State = Upper(luaL_checkstring(L, 1));
		auto CalledBy = luaL_checkstring(L, 2);
		auto File = luaL_checkstring(L, 3);
		AStates[State] = True_AQS_State::Load(&JCRPackage, File, CalledBy);
		return 0;
	}

	static int AQSA_Kill(lua_State* L) {
		auto State = Upper(luaL_checkstring(L, 1));
		auto CalledBy = luaL_checkstring(L, 2);
		if (AStates.count(State)) {
			AStates.erase(State);
			lua_pushboolean(L, true);
		} else lua_pushboolean(L, false);
		return 1;
	}

	static int AQSA_Call(lua_State* L) {
		auto State = Upper(luaL_checkstring(L, 1));
		auto CalledBy = luaL_checkstring(L, 2);
		auto Chunk = luaL_checkstring(L, 3);
		if (!AStates.count(State)) Crash(State + " is a non-existent AQS state", CalledBy, Apollo_State::TraceBack(CalledBy), 8000);
		auto St{ AStates[State] };
		int n = lua_gettop(L);
		St->ResetParams();
		for (int i = 4; i < n; i++) {
			if (lua_isinteger(L, i))
				St->ParamInt(luaL_checkinteger(L, i));
			else if (lua_isstring(L, i))
				St->ParamString(luaL_checkstring(L, i));
			else if (lua_isnumber(L, i))
				St->ParamString(to_string(luaL_checknumber(L, i)));
			else if (lua_isnil(L, i)) {	} // nothing should happen then.
			else
				Crash(string("Parameter #") + to_string(i-3) + "/"+to_string(n-3)+" is of a type that cannot be parsed onto AQS (" + to_string(lua_type(L, i)) + "/"+lua_typename(L,lua_type(L,i))+")", CalledBy, Apollo_State::TraceBack(CalledBy), 12345);
		}
		St->Call(Chunk, CalledBy);
		return 0;
	}

	static int AQSA_Have(lua_State* L) {
		auto State = Upper(luaL_checkstring(L, 1));
		lua_pushboolean(L, AStates.count(State));
		return 1;
	}

	static int AQSA_HasChunk(lua_State* L) {
		auto
			State{ Upper(luaL_checkstring(L, 1)) },
			CalledBy{ string(luaL_checkstring(L,2)) },
			Chunk{ Upper(luaL_checkstring(L, 3)) };
		if (!AStates.count(State)) Crash(State + " is a non-existent AQS state", CalledBy, Apollo_State::TraceBack(CalledBy), 8000);
		lua_pushboolean(L, AStates[State]->HasChunk(Chunk));
		return 1;
	}


	void ApolloAPIInit_AQS() {
		AQS_Panic = Paniek;
		Apollo_State::RequireNeil("API/AQS.neil");
		Apollo_State::RequireFunction("AQSA_Load", AQSA_Load);
		Apollo_State::RequireFunction("AQSA_Kill", AQSA_Kill);
		Apollo_State::RequireFunction("AQSA_Call", AQSA_Call);
		Apollo_State::RequireFunction("AQSA_have", AQSA_Have);
		Apollo_State::RequireFunction("AQSA_HasChunk", AQSA_HasChunk);
	}
}