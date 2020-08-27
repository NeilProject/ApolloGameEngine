#include <AP_Lua_CPP.hpp>
#include <TQSE.hpp>
#include <States.hpp>

namespace Tricky_Apollo {

	using namespace TrickyUnits;
	
	static int AEA_Poll(lua_State* L) {
		TQSE_Poll();
	}

	static int AEA_KeyDown(lua_State* L) {
		auto i = luaL_checknumber(L, 1);
		auto r = TQSE_KeyDown(i);
		lua_pushnumber(L, r);
		return 1;
	}


	static int AEA_KeyHit(lua_State* L) {
		auto i = luaL_checknumber(L, 1);
		auto r = TQSE_KeyHit(i);
		lua_pushnumber(L, r);
		return 1;
	}


	void ApolloAPIInit_Events(){
		Apollo_State::RequireFunction("AEA_Poll", AEA_Poll);
		Apollo_State::RequireFunction("AEA_KeyDown", AEA_KeyDown);
		Apollo_State::RequireFunction("AEA_KeyHit", AEA_KeyHit);
	}
}