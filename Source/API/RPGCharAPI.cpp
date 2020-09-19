// Lic:
// Apollo
// RPG Char API
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
// Version: 20.09.19
// EndLic

#undef blabbermouth

#include <AP_Lua_CPP.hpp>
#include <RPGStat.hpp>
#include <States.hpp>


namespace Tricky_Apollo {

	using namespace TrickyUnits;

	static void RPGError(std::string emessage) {
		Crash(emessage);
	}

	static int RPGGetParty(lua_State* L) {
		lua_pushstring(L,Party::Member(luaL_checkinteger(L, 1)).c_str());
		return 1;
	}

	static int RPGSetParty(lua_State* L) {
		auto slot = luaL_checkinteger(L, 1);
		auto ch = luaL_checkstring(L, 2);
		cout << "Gonna set " << ch << " to slot " << slot << "\n";
		Party::Member(
			slot,
			ch
		);
		return 0;
	}

	static int RPGSetPartyMax(lua_State* L) {
		Party::Max(luaL_checkinteger(L, 1));
		return 0;
	}

	static int RPGCreate(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		if (Character::Map.count(ch)) Character::Map.erase(ch);
		Character::CreateChar(ch);
		Character::Map[ch].Name = luaL_optstring(L, 2, ch.c_str());
		return 0;
	}

	static int RPGGetStatValue(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string stat = luaL_checkstring(L, 2);
		lua_pushinteger(L, Character::Map[ch].GetStat(stat)->Value());
		return 1;
	}

	static int RPGSetStatValue(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string stat = luaL_checkstring(L, 2);
		auto value = luaL_checkinteger(L, 3);
		Character::Map[ch].GetStat(stat)->Value(value);
#ifdef blabbermouth
		cout << "CHAR: " << ch << "; Defined stat: " << stat << "; value is now " << Character::Map[ch].GetStat(stat)->Value() << "; Value must be: "<<value<<"! \n";
#endif
		return 0;
	}

	static int RPGGetData(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string stat = luaL_checkstring(L, 2);
		lua_pushstring(L, Character::Map[ch].GetData(stat)->Value.c_str());
		return 1;
	}

	static int RPGSetData(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string stat = luaL_checkstring(L, 2);
		Character::Map[ch].GetData(stat,true)->Value = luaL_checkstring(L, 3);
		return 0;
	}



	static int RPGSetPoints(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string stat = luaL_checkstring(L, 2);
		std::string field = Upper(luaL_checkstring(L, 3));
		int value{ 0 }; if (Upper(field) != "MAXCOPY") value = luaL_checkinteger(L, 4);
		auto pnt = Character::Map[ch].GetPoints(stat);
		if (field == "HAVE")
			pnt->Have(value);
		else if (field == "MAXIMUM" || field == "MAXI" || field == "MAX")
			pnt->Maxi(value);
		else if (field == "MINUMUM" || field == "MINI" || field == "MIN")
			pnt->Mini(value);
		else if (field == "MAXCOPY")
			pnt->MaxCopy(luaL_checkstring(L, 4));
		return 0;
	}

	static int RPGGetPoints(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string stat = luaL_checkstring(L, 2);
		std::string field = Upper(luaL_checkstring(L, 3));
		auto pnt = Character::Map[ch].GetPoints(stat);
		if (field == "HAVE")
			lua_pushinteger(L,pnt->Have());
		else if (field == "MAXIMUM" || field == "MAXI" || field == "MAX")
			lua_pushinteger(L,pnt->Maxi());
		else if (field == "MINUMUM" || field == "MINI" || field == "MIN")
			lua_pushinteger(L,pnt->Mini());
		else if (field == "MAXCOPY")
			lua_pushstring(L,pnt->MaxCopy().c_str());
		return 1;
	}

	static int RPGKillList(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string list = luaL_checkstring(L, 2);
		Character::Map[ch].NULLList(list);
		return 0;
	}

	static int RPGListCount(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string list = luaL_checkstring(L, 2);
		lua_pushinteger(L,Character::Map[ch].GetList(list)->List.size());
		return 1;
	}

	static int RPGListGet(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string list = luaL_checkstring(L, 2);
		int index = luaL_checkinteger(L, 3);
		if (index < 0 || index >= Character::Map[ch].GetList(list)->List.size()) { Crash("RPG list index out of bounds!"); }
		lua_pushstring(L, Character::Map[ch].GetList(list)->List[index].c_str());
		return 1;
	}

	static int RPGListSet(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string list = luaL_checkstring(L, 2);
		int index = luaL_checkinteger(L, 3);
		std::string value = luaL_checkstring(L, 4);
		if (index < 0 || index >= Character::Map[ch].GetList(list)->List.size()) { Crash("RPG list index out of bounds!"); }
		Character::Map[ch].GetList(list)->List[index] = value;
		return 0;
	}

	static int RPGListAdd(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string list = luaL_checkstring(L, 2);		
		std::string value = luaL_checkstring(L, 3);		
		Character::Map[ch].GetList(list)->List.push_back(value);
		return 0;
	}

	static int RPGSetStatScript(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string stat = luaL_checkstring(L, 2);
		std::string value = luaL_checkstring(L, 3);
		Character::Map[ch].GetStat(stat)->Script(value);
	}

	static int RPGGetAllStats(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		lua_pushstring(L, Character::Map[ch].StatList().c_str());
		return 1;
	}


	void ApolloAPIInit_RPGCharAPI() {
		Character::Panic = RPGError;
		Apollo_State::RequireFunction("RPGGetParty", RPGGetParty);
		Apollo_State::RequireFunction("RPGSetParty", RPGSetParty);
		Apollo_State::RequireFunction("RPGCreate", RPGCreate);
		Apollo_State::RequireFunction("RPGGetStatValue", RPGGetStatValue);
		Apollo_State::RequireFunction("RPGSetStatValue", RPGSetStatValue);
		Apollo_State::RequireFunction("RPGSetPoints", RPGSetPoints);
		Apollo_State::RequireFunction("RPGGetPoints", RPGGetPoints);
		Apollo_State::RequireFunction("RPGGetData", RPGGetData);
		Apollo_State::RequireFunction("RPGSetData", RPGSetData);
		Apollo_State::RequireFunction("RPGKillList", RPGKillList);
		Apollo_State::RequireFunction("RPGListCount", RPGListCount);
		Apollo_State::RequireFunction("RPGSetStatScript", RPGSetStatScript);
		Apollo_State::RequireFunction("RPGSetPartyMax", RPGSetPartyMax);
		Apollo_State::RequireFunction("RPGGetAllStats", RPGGetAllStats);
		Apollo_State::RequireLua("API/RPGStat.lua");
	}

}