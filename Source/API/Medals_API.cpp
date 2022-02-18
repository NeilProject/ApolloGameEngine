#include <AP_Lua_CPP.hpp>
#include <Medals.hpp>
#include "../../Headers/Globals.hpp"
#include "../../Headers/States.hpp"

using namespace TrickyMedals;
namespace Tricky_Apollo {

	static char gameid[200];
	static char StorageMethod[200];

	static int Medal_SetGame(lua_State* L) { strcpy_s(gameid, luaL_checkstring(L, 1)); return 0; }
	static int Medal_GetGame(lua_State* L) { lua_pushstring(L, gameid); return 1; }
	static int Medal_Award(lua_State* L) { Award(gameid,luaL_checkstring(L, 1));  return 0; }
	static int Medal_FullScore(lua_State* L) { lua_pushinteger(L,FullScore()); return 1; }
	static int Medal_MaxScore(lua_State* L) { lua_pushinteger(L, MaxScore()); return 1; }
	static int Medal_Percent(lua_State* L) { lua_pushinteger(L, ScorePercent()); return 1; }
	static int Medal_Awarded(lua_State* L) { lua_pushboolean(L, Awarded(gameid, luaL_checkstring(L, 1))); return 1; }


	static int Medal_SetStorage(lua_State* L) { strcpy_s(StorageMethod, luaL_checkstring(L, 1)); Storage(StorageMethod); return 0; }
	static int Medal_GetStorage(lua_State* L) { lua_pushstring(L, StorageMethod); return 1; }

	void ApolloAPIInit_Medals() {
		LoadFromInternet();
		strcpy_s(gameid, "");
		strcpy_s(StorageMethod, "Store");
		Storage(StorageMethod);

		if (JCRPackage.EntryExists("ID/Medals/Medals.ini")) {
			LoadFromJCR6(&JCRPackage, "ID/Medals/Medals.ini");
		}	

		Apollo_State::RequireFunction("AMDL_SetGame", Medal_SetGame);
		Apollo_State::RequireFunction("AMDL_Game", Medal_GetGame);
		Apollo_State::RequireFunction("AMDL_Award", Medal_Award);
		Apollo_State::RequireFunction("AMDL_Awarded", Medal_Awarded);
		Apollo_State::RequireFunction("AMDL_FullScore", Medal_FullScore);
		Apollo_State::RequireFunction("AMDL_MaxScore", Medal_MaxScore);
		Apollo_State::RequireFunction("AMDL_Percent", Medal_Percent);
		Apollo_State::RequireFunction("AMDL_Storage", Medal_GetStorage);
		Apollo_State::RequireFunction("AMDL_SetStorage", Medal_SetStorage);
		Apollo_State::RequireNeil("API/Medals.neil");
	}

}