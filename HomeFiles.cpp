// Lic:
// Home Files
// Apollo
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
// Version: 22.07.19
// EndLic
// C++
#include <string>

// Lua
#include <AP_Lua_CPP.hpp>

// Tricky's Units
#include <Dirry.hpp>
#include <QuickStream.hpp>

// Apollo
#include <Crash.hpp>
#include <Globals.hpp>
#include <States.hpp>
#include <Identify.hpp>
#include <FileList.hpp>


using namespace std;
using namespace TrickyUnits;

namespace Tricky_Apollo {

	inline string SGDir() {
		return  Dirry("$Home$/ApolloGameData/" + Identify::ProjectData("ID"));
	}

	static int AHFA_Path(lua_State* L) {
		lua_pushstring(L, SGDir().c_str());
		return 1;
	}

	static void AllowPath(string a,string S) {
		a = TReplace(a, '\\', '/');
		if (prefixed(a, "../")) Crash("Invalid dir!", S);
		for (int i = 1; i < a.size() - 4; ++i)
			if (mid(a, i + 1, 4) == "/../") Crash("Invalid dir!", S);
	}

	static bool BAllowPath(string a, string S) {
		a = TReplace(a, '\\', '/');
		if (prefixed(a, "../")) return false;
		for (int i = 1; i < a.size() - 4; ++i)
			if (mid(a, i + 1, 4) == "/../") return false;
		return true;
	}


	static int AHFA_Load(lua_State* L) {
		auto
			State{ luaL_checkstring(L,1) },
			File{ luaL_checkstring(L,2) };
		auto
			MustExist{ luaL_optinteger(L,3,1) };
		auto RF{ SGDir() + "/" + File };
		AllowPath(RF,State);
		if (!FileExists(RF)) {
			if (MustExist) {
				Crash("Home file not found: " + string(File), State, Apollo_State::TraceBack(State), 1337);
			}
			lua_pushstring(L, "");
			return 1;
		}
		lua_pushstring(L, LoadString(RF).c_str());
		return 1;
	}

	static int AHFA_Save(lua_State* L) {
		auto
			State{ luaL_checkstring(L,1) },
			File{ luaL_checkstring(L,2) },
			Data{ luaL_checkstring(L,3) };
		auto
			RF{ SGDir() + "/" + File };
		AllowPath(RF,State);
		SaveString(RF, Data);
		return 0;
	}

	static int AHFA_Exists(lua_State* L) {
		auto
			State{ luaL_checkstring(L,1) },
			File{ luaL_checkstring(L,2) };
		auto RF{ SGDir() + "/" + File };
		AllowPath(RF,State);
		lua_pushboolean(L, FileExists(RF));
		return 1;
	}

	static int AHFA_DirExists(lua_State* L) {
		auto
			State{ luaL_checkstring(L,1) },
			Dir{ luaL_checkstring(L,2) };
		auto 
			RF{ SGDir() + "/" + Dir };
		AllowPath(RF, State);
		lua_pushboolean(L, DirectoryExists(RF));
		return 1;
	}

	static int AHFA_GetDir(lua_State* L) {
		auto
			State{ luaL_checkstring(L,1) },
			Dir{ luaL_checkstring(L,2) };
		auto
			RF{ SGDir() + "/" + Dir };	AllowPath(RF, State);
		auto
			F = GetTree(RF);
		string
			Ret{ "" };
		for (auto fl : F) {
			if (Ret.size()) Ret += ":";
			Ret += fl;
		}
		lua_pushstring(L, Ret.c_str());
		return 1;
	}

	static int AHFA_CreateDir(lua_State* L) {
		auto
			State{ luaL_checkstring(L,1) },
			Dir{ luaL_checkstring(L,2) };
		auto
			RF{ SGDir() + "/" + Dir };	AllowPath(RF, State);
		lua_pushboolean(L,MakeDir(RF));
		return 1;
	}

	static int AHFA_Write(lua_State* L) {
		auto
			State{ luaL_checkstring(L,1) },
			File{ luaL_checkstring(L,2) },
			Str{ luaL_checkstring(L,3) };
		auto
			RF{ SGDir() + "/" + File };	
		auto
			AP{ BAllowPath(RF, State) };
		if (!AP) {
			lua_pushboolean(L, false);
			lua_pushstring(L, string(string("Invalid path: ")+File).c_str());
			return 2;
		}
		if (!DirectoryExists(ExtractDir(RF))) {
			lua_pushboolean(L, false);
			lua_pushstring(L, string(string("Path not found: ") + File).c_str());
			return 2;
		}
		SaveString(RF, Str);
		lua_pushboolean(L, true);
		lua_pushstring(L, "");
		return 2;
	}

	// Init Apollo Home File API
	void ApolloAPIInit_AHFA() {
		Apollo_State::RequireFunction("AHFA_PATH", AHFA_Path);
		Apollo_State::RequireFunction("AHFA_LOAD", AHFA_Load);
		Apollo_State::RequireFunction("AHFA_SAVE", AHFA_Save);
		Apollo_State::RequireFunction("AHFA_EXISTS", AHFA_Exists);
		Apollo_State::RequireFunction("AHFA_DIREXISTS", AHFA_DirExists);
		Apollo_State::RequireFunction("AHFA_GETDIR", AHFA_GetDir);
		Apollo_State::RequireFunction("AHFA_CREATEDIR", AHFA_CreateDir);
		Apollo_State::RequireFunction("AHFA_WRITE", AHFA_Write);
		Apollo_State::RequireNeil("API/HomeFile.neil");
	}

}