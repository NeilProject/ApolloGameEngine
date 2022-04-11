// Lic:
// Apollo Game Engine
// Game Vars
// 
// 
// 
// (c) Jeroen P. Broks, 2021
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
// Version: 21.07.07
// EndLic
#include <AP_Lua_CPP.hpp>
#include <map>
#include <string>
#include <States.hpp>
#include <Crash.hpp>
#include <QuickString.hpp>
#include <TrickySTOI.hpp>

using namespace std; // I know! I'm horrible! You don't have to tell me!
using namespace TrickyUnits;

namespace Tricky_Apollo {

	typedef struct tgv {
		map<string, int> gInt;
		map<string, string> gString;
		map<string, bool> gBool;
	} tgv;
	static map<string, tgv> bgv{};
	// Allow use in AQS
	map<string, int>* xgInt(string bundle) { return &(bgv[bundle].gInt); }
	map<string, string>* xgStr(string bundle) { return &(bgv[bundle].gString); }
	map<string, bool>* xgBoo(string bundle) { return &(bgv[bundle].gBool); }

	static int AGV_Define(lua_State* L) {
		auto state{ luaL_checkstring(L,1) };
		auto bundle{ luaL_checkstring(L,2) };
		auto ttype{ luaL_checkinteger(L,3) };
		auto key{ Upper(luaL_checkstring(L,4)) };
		switch (ttype) {
		case 1: {
			// Boolean
			auto value{ luaL_checkinteger(L,5) };
			bgv[bundle].gBool[key] = value;
			return 0;
		}
		case 2: {
			// Integer
			auto value{ luaL_checkinteger(L,5) };
			bgv[bundle].gInt[key] = value;
			return 0;
		}
		case 3: {
			// String
			auto value{ luaL_checkstring(L,5) };
			bgv[bundle].gString[key] = value;
			return 0;
		}
		default:
			Crash("S:Unknown GameVar type (" + to_string(ttype) + ")", state);
			return 0;
		}
	}

	static int AGV_Receive(lua_State* L) {
		auto state{ luaL_checkstring(L,1) };
		auto bundle{ luaL_checkstring(L,2) };
		auto ttype{ luaL_checkinteger(L,3) };
		auto key{ Upper(luaL_checkstring(L,4)) };
		switch (ttype) {
		case 1: {
			// Boolean
			lua_pushboolean(L, bgv[bundle].gBool[key]);
			return 1;
		}
		case 2: {
			// Integer
			lua_pushinteger(L, bgv[bundle].gInt[key]);
			return 1;
		}
		case 3: {
			// String
			lua_pushstring(L, bgv[bundle].gString[key].c_str());
			return 1;
		}
		default:
			Crash("G:Unknown GameVar type (" + to_string(ttype) + ")", state);
			return 0;
		}
	}

	static int AGV_Clear(lua_State* L) {
		auto state{ luaL_checkstring(L,1) };
		auto bundle{ luaL_checkstring(L,2) };
		bgv[bundle].gBool.clear();
		bgv[bundle].gInt.clear();
		bgv[bundle].gString.clear();
		return 0;
	}

	static int AGV_ClearAll(lua_State* L) {
		bgv.clear();
		return 0;
	}

	static string Enroll(string Bundle, string vn, string l) {
		string ret{ "" };
		string p{ "" };
		if (l == "lua") ret += "-- "; else ret += "// ";
		ret += "Apollo Game var dump out! (Target: " + l + ")\n\n";
		if (l == "neil") { ret += "Init\n"; p = "\t"; }
		ret += p + "gv.Clear()\n";
		for (auto fe : bgv[Bundle].gBool) { ret += p + vn + ".vbool[\"" + fe.first + "\"] = "; if (fe.second) ret += "true\n"; else ret += "false\n"; }
		for (auto fe : bgv[Bundle].gInt) { ret += p + vn + ".vint[\"" + fe.first + "\"] = " + to_string(fe.second) + "\n"; }
		for (auto fe : bgv[Bundle].gString) { ret += p + vn + ".vstr[\"" + fe.first + "\"] = \"" + fe.second + "\"\n"; }
		if (l == "neil") { ret += "End\n"; }
		return ret;
	}

	static int AGV_Enroll(lua_State* L) {
		auto state{ luaL_checkstring(L,1) };
		auto bundle{ luaL_checkstring(L,2) };
		auto name{ luaL_checkstring(L,3) };
		auto lang{ luaL_optstring(L,4,"neil") };
		lua_pushstring(L, Enroll(bundle, name, lang).c_str());
		return 1;
	}

	static int AGV_Show(lua_State* L) {
		auto state{ luaL_checkstring(L,1) };
		auto bundle{ luaL_checkstring(L,2) };
		string ret{ "" };
		for (auto fe : bgv[bundle].gBool) {
			ret += "Bool " + fe.first + " = ";
			if (fe.second) ret += "True\n"; else ret += "False\n";
		}
		for (auto fe : bgv[bundle].gInt) ret += "Int " + fe.first + " = " + to_string(fe.second)+"\n";
		for (auto fe : bgv[bundle].gString) ret += "String " + fe.first + " = \"" + fe.second + "\"\n";		
		lua_pushstring(L, ret.c_str());
		return 1;
	}

	static int AGV_Sub(lua_State* L) {
		static string tf[2] { "false","true" };
		auto state{ luaL_checkstring(L,1) };
		auto bundle{ luaL_checkstring(L,2) };
		string ret{ luaL_checkstring(L,3) };
		for (auto fe : bgv[bundle].gBool) ret = TReplace(ret, string("&" + fe.first), tf[fe.second]);
		for (auto fe : bgv[bundle].gInt) ret = TReplace(ret, string("%" + fe.first), to_string(fe.second));
		for (auto fe : bgv[bundle].gString) ret = TReplace(ret, string("$" + fe.first), fe.second);
		lua_pushstring(L, ret.c_str());
		return 1;
	}

	static int AGV_ToGINIE(lua_State* L) {
		static string tf[2]{ "false","true" };
		auto state{ luaL_checkstring(L,1) };
		auto bundle{ luaL_checkstring(L,2) };
		string ret{ "" };
		ret += "[bool]\n"; for (auto fe : bgv[bundle].gBool) ret += fe.first + "=" + tf[fe.second]+"\n";
		ret += "\n[int]\n"; for (auto fe : bgv[bundle].gInt) ret += fe.first + "=" + to_string(fe.second) + "\n";
		ret += "\n[string]\n"; for (auto fe : bgv[bundle].gString) ret += fe.first + "=" + fe.second + "\n";
		lua_pushstring(L, ret.c_str());
		return 1;
	}

	static int AGV_FromGINIE(lua_State* L) {
		GINIE Data;
		auto state{ luaL_checkstring(L,1) };
		auto bundle{ luaL_checkstring(L,2) };
		auto source{ luaL_checkstring(L,3) };
		Data.Parse(source);
		if (bgv.count(bundle)) bgv.erase(bundle);
		for (auto k : Data.EachValue("bool")) bgv[bundle].gBool[k] = Upper(Data.Value("bool", k)) == "TRUE";
		for (auto k:Data.EachValue("int")) bgv[bundle].gInt[k] = ToInt(Data.Value("int", k));
		for (auto k : Data.EachValue("string")) bgv[bundle].gString[k] = Data.Value("string", k);
		return 0;
	}
	

	void ApolloAPIInit_InitGameVars() {
		Apollo_State::RequireFunction("AGVR_Define", AGV_Define);
		Apollo_State::RequireFunction("AGVR_Receive", AGV_Receive);
		Apollo_State::RequireFunction("AGVR_Clear", AGV_Clear);
		Apollo_State::RequireFunction("AGVR_ClearAll", AGV_ClearAll);
		Apollo_State::RequireFunction("AGVR_Enroll", AGV_Enroll);
		Apollo_State::RequireFunction("AGVR_Show", AGV_Show);
		Apollo_State::RequireFunction("AGVR_Sub", AGV_Sub);
		Apollo_State::RequireFunction("AGVR_ToGINIE", AGV_ToGINIE);
		Apollo_State::RequireFunction("AGVR_FrGINIE", AGV_FromGINIE);
		Apollo_State::RequireNeil("API/GameVars.neil");
	}

}