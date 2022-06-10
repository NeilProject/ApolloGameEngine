// Lic:
// Apollo Game Engine
// GINIE API
// 
// 
// 
// (c) Jeroen P. Broks, 2022
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
// Version: 22.05.10
// EndLic
#include <AP_Lua_CPP.hpp>
#include <GINIE.hpp>
#include <Globals.hpp>
#include <States.hpp>

#define bcheck(r)\
	auto \
		state{luaL_checkstring(L,1)},\
		GID{ luaL_checkstring(L,2) };\
		if (!GData.count(GID)) { Crash(string("GINIE data for script not found on tag: ")+GID,state,Apollo_State::TraceBack(state)); return r;}

#define ocheck()\
	auto \
		state{luaL_checkstring(L,1)},\
		GID{ luaL_optstring(L,2,"") }

using namespace std;
using namespace TrickyUnits;

namespace Tricky_Apollo {

	typedef unsigned int uint;

	uint count{ 0 };
	static map<string, GINIE> GData;

	static int GCreate(lua_State* L) {
		ocheck();
		auto S{ luaL_optstring(L,3,"") };
		auto T{ luaL_optinteger(L,4,0) };
		char Tag[255];
		if (strcmp(GID, "") == 0) {
			do {
				sprintf_s(Tag, "GINIE_%08X", count++);
			} while (GData.count(Tag));
		} else {
			int i = 0;
			do { Tag[i] = GID[i]; } while (GID[i++]);
		}
		GData[Tag] = GINIE();
		switch (T) {
		case 0:
			break;
		case 1: {
			auto s = JCRPackage.String(S);
			GData[Tag].Parse(s);
			break;
		}
		case 2: {
			auto s = ARF.String(S);
			GData[Tag].Parse(s);
			break;
		}
		case 3: {
			GData[Tag].Parse(S);
			break;
		}
		case 4: {
			auto b = JCRPackage.Characters(S);
			GData[Tag].AutoParse(b);
			break;
		}
		default:
			Crash(string("Unknown GINIE creation code ") + to_string(T), state, Apollo_State::TraceBack(state));
		}
		lua_pushstring(L,Tag);
		return 1;
	}

	static int GSetValue(lua_State* L) {
		bcheck(0);
		auto
			cat{ luaL_checkstring(L,3) },
			key{ luaL_checkstring(L,4) },
			val{ luaL_checkstring(L,5) };
		GData[GID].Value(cat, key, val);
		return 0;
	}

	static int GGetValue(lua_State* L) {
		bcheck(0);
		auto
			cat{ luaL_checkstring(L,3) },
			key{ luaL_checkstring(L,4) };
		lua_pushstring(L,GData[GID].Value(cat, key).c_str());
		return 1;
	}

	static int GUnParse(lua_State* L) {
		bcheck(0);
		lua_pushstring(L, GData[GID].UnParse().c_str());
		return 1;
	}

	static int GListCnt(lua_State* L) {
		bcheck(0);
		auto
			cat{ luaL_checkstring(L,3) },
			key{ luaL_checkstring(L,4) };
		lua_pushinteger(L, GData[GID].List(cat, key)->size());
		return 1;
	}

	static int GListGet(lua_State* L) {
		bcheck(0);
		auto
			cat{ luaL_checkstring(L,3) },
			key{ luaL_checkstring(L,4) };
		auto
			idx{ luaL_checkinteger(L,5) };
		lua_pushstring(L, (*GData[GID].List(cat, key))[idx].c_str());
		return 1;
	}

	static int GListAdd(lua_State* L) {
		bcheck(0);
		auto
			cat{ luaL_checkstring(L,3) },
			key{ luaL_checkstring(L,4) },
			val{ luaL_checkstring(L,5) };
		GData[GID].Add(cat, key, val);
		return 0;
	}

	static int GHas(lua_State* L) {
		auto 
			state{ luaL_checkstring(L,1) }, 
			GID{ luaL_checkstring(L,2) }; 
		lua_pushboolean(L, GData.count(GID));
		return 1;
	}

	static int GCategories(lua_State* L) {
		auto
			state{ luaL_checkstring(L,1) },
			GID{ luaL_checkstring(L,2) };
		string r{ "" };
		for (auto k : GData[GID].EachGroup()) {
			if (r.size()) r += "\n";
			r += k;
		}
		lua_pushstring(L, r.c_str());
		return 1;		
	}

	




	void ApolloAPIInit_GINIE() {
		Apollo_State::RequireFunction("AGIN_Create", GCreate);
		Apollo_State::RequireFunction("AGIN_SetVal", GSetValue);
		Apollo_State::RequireFunction("AGIN_GetVal", GGetValue);
		Apollo_State::RequireFunction("AGIN_UnPars", GUnParse);
		Apollo_State::RequireFunction("AGIN_ListCn", GListCnt);
		Apollo_State::RequireFunction("AGIN_listGt", GListGet);
		Apollo_State::RequireFunction("AGIN_ListAd", GListAdd);
		Apollo_State::RequireFunction("AGIN_Categories", GCategories);
		Apollo_State::RequireFunction("AGIN_Has", GHas);		
		Apollo_State::RequireNeil("API/GINIE.neil");
	}
}