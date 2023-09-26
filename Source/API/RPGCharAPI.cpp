// Lic:
// Apollo
// RPG Char API
// 
// 
// 
// (c) Jeroen P. Broks, 2020, 2021, 2022
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
// Version: 22.04.30
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

	static void CheckChar(std::string ch) {
		if (!Character::Map.count(ch)) Crash("Character " + ch + " is non-existent");
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

	static int RPGGetPartyMax(lua_State* L) {
		lua_pushinteger(L, Party::Max());
		return 1;
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

	static int RPGHasStat(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string stat = luaL_checkstring(L, 2);
		lua_pushboolean(L, Character::Map[ch].HasStat(stat));
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
		static bool Chat = false;
		std::string ch = luaL_checkstring(L, 1);
		std::string stat = luaL_checkstring(L, 2);
		std::string field = Upper(luaL_checkstring(L, 3));
		int value{ 0 }; if (Upper(field) != "MAXCOPY") value = luaL_checkinteger(L, 4);
		CheckChar(ch);
		auto pnt = Character::Map[ch].GetPoints(stat);
		if (Chat)
			cout << "POINTS CHANGE> " << ch << "." << stat << "." << field << " => " << value << endl;
		if (field == "HAVE")
			pnt->Have(value);
		else if (field == "MAXIMUM" || field == "MAXI" || field == "MAX")
			pnt->Maxi(value);
		else if (field == "MINIMUM" || field == "MINI" || field == "MIN")
			pnt->Mini(value);
		else if (field == "MAXCOPY")
			pnt->MaxCopy(luaL_checkstring(L, 4));
		else if (field == "CHAT")
			Chat = value > 0;
		else
			Crash("Unknown field: " + field + " for points " + stat + " (" + ch + ") (set)");
		return 0;
	}

	static int RPGGetPoints(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string stat = luaL_checkstring(L, 2);
		std::string field = Upper(luaL_checkstring(L, 3));
		CheckChar(ch);
		auto pnt = Character::Map[ch].GetPoints(stat);
		if (field == "HAVE")
			lua_pushinteger(L, pnt->Have());
		else if (field == "MAXIMUM" || field == "MAXI" || field == "MAX")
			lua_pushinteger(L, pnt->Maxi());
		else if (field == "MINIMUM" || field == "MINI" || field == "MIN")
			lua_pushinteger(L, pnt->Mini());
		else if (field == "MAXCOPY")
			lua_pushstring(L, pnt->MaxCopy().c_str());
		else
			Crash("Unknown field: " + field + " for points " + stat + " (" + ch + ") (get)");
		return 1;
	}

	static int RPGKillList(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string list = luaL_checkstring(L, 2);
		CheckChar(ch);
		Character::Map[ch].NULLList(list);
		return 0;
	}

	static int RPGListCount(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string list = luaL_checkstring(L, 2);
		CheckChar(ch);
		lua_pushinteger(L,Character::Map[ch].GetList(list)->List.size());
		return 1;
	}

	static int RPGListHas(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string list = luaL_checkstring(L, 2);
		std::string needle = luaL_checkstring(L, 3);
		bool ignorecase = luaL_optinteger(L, 4, 0);
		bool ret = false;
		CheckChar(ch);
		for(auto &D : Character::Map[ch].GetList(list)->List){
			ret = ret || D == needle;
			if (ignorecase) ret = ret || Upper(D) == Upper(needle);
		}
		lua_pushboolean(L, ret);
		return 1;
	}

	static int RPGListGet(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string list = luaL_checkstring(L, 2);
		int index = luaL_checkinteger(L, 3);
		CheckChar(ch);
		if (index < 0 || index >= Character::Map[ch].GetList(list)->List.size()) { Crash("RPG list index out of bounds!"); }
		lua_pushstring(L, Character::Map[ch].GetList(list)->List[index].c_str());
		return 1;
	}

	static int RPGListSet(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1);
		std::string list = luaL_checkstring(L, 2);
		int index = luaL_checkinteger(L, 3);
		std::string value = luaL_checkstring(L, 4);
		CheckChar(ch);
		if (index < 0 || index >= Character::Map[ch].GetList(list)->List.size()) { Crash("RPG list index out of bounds!"); }
		Character::Map[ch].GetList(list)->List[index] = value;
		return 0;
	}

	static int RPGListAdd(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1); CheckChar(ch);
		std::string list = luaL_checkstring(L, 2);		
		std::string value = luaL_checkstring(L, 3);		
		Character::Map[ch].GetList(list)->List.push_back(value);
		return 0;
	}	

	static int RPGSetStatScript(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1); CheckChar(ch);
		std::string stat = luaL_checkstring(L, 2);
		std::string value = luaL_checkstring(L, 3);
		Character::Map[ch].GetStat(stat)->Script(value);
		return 0;
	}

	static int RPGGetAllStats(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1); CheckChar(ch);
		lua_pushstring(L, Character::Map[ch].StatList().c_str());
		return 1;
	}

	static int RPGGetStatList(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1); CheckChar(ch);
		string r{ "" };
		for (auto s : Character::Map[ch].Stats()) {
			if (r.size()) r += ";";
			r += s;
		}
		lua_pushstring(L, r.c_str());
		return 1;
	}

	static int RPGGetChName(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1); CheckChar(ch);
		lua_pushstring(L, Character::Map[ch].Name.c_str());
		return 1;
	}

	static int RPGSetChName(lua_State* L) {
		std::string ch = luaL_checkstring(L, 1); CheckChar(ch);
		Character::Map[ch].Name = luaL_checkstring(L, 2);
		return 0;
	}

	static int RPGKillChar(lua_State* L) {
		auto ch = luaL_checkstring(L, 1); //CheckChar(ch);
		auto prefix = luaL_optinteger(L, 2, 0);
		if (prefix) {
			std::vector<string>Kill{};
			for (auto& s : Character::Map) if (prefixed(s.first, ch)) Kill.push_back(s.first);
			for (auto& s : Kill) Character::Map.erase(s);
		} else {
			if (Character::Map.count(ch)) Character::Map.erase(ch);
		}
		return 0;
	}

#define Lnk(func,kind,lnk)  \
	static int func(lua_State* L) {	\
			auto src = luaL_checkstring(L, 1); \
			auto tgt = luaL_checkstring(L, 2); \
			auto dat = luaL_checkstring(L, 3); \
			if (!Character::Map.count(src)) Crash("Character Link "+string(kind)+" Error: Source " + string(src) + " non existent!"); \
			if (!Character::Map.count(tgt)) Crash("Character Link "+string(kind)+" Error: Target " + string(tgt) + " non existent!"); \
			Character::Map[tgt].lnk(dat, src); \
			return 0; \
	}
	Lnk(RPGLinkStat, "Stat", LinkStat);
	Lnk(RPGLinkData, "Data", LinkData);
	Lnk(RPGLinkList, "List", LinkList);
	Lnk(RPGLinkPoints, "Points", LinkPoints);

	static int RPGHasChar(lua_State* L) {
		lua_pushboolean(L, Character::Map.count(luaL_checkstring(L, 1)));
		return 1;
	}

	static int RPGAllChars(lua_State* L) {
		// Will list all characters in RAM, regardess if they are in the party or not.
		std::string ret{ "" };
		for (auto& i : Character::Map) {
			if (ret.size()) ret += ";";
			ret += i.first;
		}
		lua_pushstring(L, ret.c_str());
		return 1;
	}

	void ApolloAPIInit_RPGCharAPI() {
		Character::Panic = RPGError;
		Apollo_State::RequireFunction("RPGGetParty", RPGGetParty);
		Apollo_State::RequireFunction("RPGSetParty", RPGSetParty);
		Apollo_State::RequireFunction("RPGCreate", RPGCreate);
		Apollo_State::RequireFunction("RPGGetStatValue", RPGGetStatValue);
		Apollo_State::RequireFunction("RPGSetStatValue", RPGSetStatValue);
		Apollo_State::RequireFunction("RPGHasStat", RPGHasStat);
		Apollo_State::RequireFunction("RPGSetPoints", RPGSetPoints);
		Apollo_State::RequireFunction("RPGGetPoints", RPGGetPoints);
		Apollo_State::RequireFunction("RPGGetData", RPGGetData);
		Apollo_State::RequireFunction("RPGSetData", RPGSetData);
		Apollo_State::RequireFunction("RPGKillList", RPGKillList);
		Apollo_State::RequireFunction("RPGListCount", RPGListCount);
		Apollo_State::RequireFunction("RPGListHas", RPGListHas);
		Apollo_State::RequireFunction("RPGListAdd", RPGListAdd);
		Apollo_State::RequireFunction("RPGListGet", RPGListGet);
		Apollo_State::RequireFunction("RPGListSet", RPGListSet);
		Apollo_State::RequireFunction("RPGSetStatScript", RPGSetStatScript);
		Apollo_State::RequireFunction("RPGSetPartyMax", RPGSetPartyMax);
		Apollo_State::RequireFunction("RPGGetPartyMax", RPGGetPartyMax);
		Apollo_State::RequireFunction("RPGGetAllStats", RPGGetAllStats);
		Apollo_State::RequireFunction("RPGGetStatList", RPGGetStatList);
		Apollo_State::RequireFunction("RPGGetChName", RPGGetChName);
		Apollo_State::RequireFunction("RPGSetChName", RPGSetChName);
		Apollo_State::RequireFunction("RPGKillChar", RPGKillChar);
		Apollo_State::RequireFunction("RPGLinkStat", RPGLinkStat);
		Apollo_State::RequireFunction("RPGLinkData", RPGLinkData);
		Apollo_State::RequireFunction("RPGLinkList", RPGLinkList);
		Apollo_State::RequireFunction("RPGLinkPoints", RPGLinkPoints);
		Apollo_State::RequireFunction("RPGHasChar", RPGHasChar);
		Apollo_State::RequireFunction("RPGAllChars", RPGAllChars);
		Apollo_State::RequireLua("API/RPGStat.lua");
	}

}