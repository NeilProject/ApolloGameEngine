// Lic:
// Source/API/JCR6.cpp
// Apollo
// version: 20.09.03
// Copyright (C) 2020 JCR6 access
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// EndLic
//C++
#include <string>

// Lua
#include <AP_Lua_CPP.hpp>

// JCR6
#include <jcr6_core.hpp>

// Apollo
#include <Crash.hpp>
#include <Globals.hpp>
#include <States.hpp>

namespace Tricky_Apollo {

	using namespace std;
	using namespace jcr6;

#define JCRCheck() \
	string res = luaL_checkstring(L, 1);\
	string State = luaL_checkstring(L, 2);\
	JT_Dir AJD;\
	JT_Dir* JD = NULL;\
	if (res[0] == '*') {\
		if (res == "*MPACKAGE") JD = &JCRPackage;\
		else Crash("Unknown * tag sent to JCR6 API", State, Apollo_State::TraceBack(State));\
	} else {\
		AJD = Dir(res);\
		JD = &AJD;\
	}



	static int Apollo_JCR6_Entries(lua_State* L) {
		JCRCheck()
		int c = 0;
		for (auto& e : JD->Entries()) {
			c++;
			lua_pushstring(L, e.second.Entry().c_str());
		}
		return c;
	}

	static int Apollo_JCR6_LoadString(lua_State* L) {
		JCRCheck();
		string Entry = luaL_checkstring(L, 3);
		lua_pushstring(L, JD->String(Entry).c_str());
		return 1;
	}

	static int Apollo_JCR6_Size(lua_State* L) {
		JCRCheck();
		string Entry = luaL_checkstring(L, 3);
		lua_pushinteger(L, JD->Entry(Entry).RealSize());
		return 1;
	}

	static int Apollo_JCR_EntryExists(lua_State* L) {
		JCRCheck();
		string Entry = luaL_checkstring(L, 3);
		lua_pushboolean(L, JD->EntryExists(Entry));
		return 1;
	}

	void ApolloAPIInit_JCR6() {
		Apollo_State::RequireFunction("AJCR_Entries", Apollo_JCR6_Entries);
		Apollo_State::RequireFunction("AJCR_LoadString", Apollo_JCR6_LoadString);
		Apollo_State::RequireFunction("AJCR_Size", Apollo_JCR6_Size);
		Apollo_State::RequireFunction("AJCR_EntryExists", Apollo_JCR_EntryExists);
		Apollo_State::RequireNeil("API/JCR6.neil");
	}
}