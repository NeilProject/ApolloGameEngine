// Lic:
// Source/API/Dialog.cpp
// Dialog API
// version: 21.03.05
// Copyright (C) 2021 Jeroen P. Broks
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
#include <AP_Lua_CPP.hpp>

// Apollo
//#include <SDL_Manager.hpp>
#include <Crash.hpp>
#include <States.hpp>

// Tricky's Units
#include <TQSE.hpp>
#include <TQSG.hpp>

#define LDEF(func) static int func(lua_State*L) 

namespace Tricky_Apollo {

	using namespace TrickyUnits;

	LDEF(Dialog_Annoy) {
		using namespace std;
		auto
			paramcount{ lua_gettop(L) };
		string msg{ "" };
		string caption{ "Apollo Game Engine" };
		auto flags{ SDL_MESSAGEBOX_INFORMATION };
		switch (paramcount) {
		case 0: 
			Crash("Dialog.Annoy requires input!");
			break;
		case 1: 
			msg = luaL_checkstring(L, 1);
			break;
		case 3:
		{
			auto flag{ Upper(luaL_checkstring(L,3)) };
			if (flag == "ERROR") flags = SDL_MESSAGEBOX_ERROR;
			else if (flag == "WARNING") flags = SDL_MESSAGEBOX_WARNING;
		}
			// FALLTHROUHG!!
		case 2:
			caption = luaL_checkstring(L, 1);
			msg = luaL_checkstring(L, 2);
		
		}
		SDL_ShowSimpleMessageBox(flags, msg.c_str(), caption.c_str(),TQSG_Window());
		return 0;
	}

	LDEF(Dialog_Yes) {
		lua_pushboolean(L, TQSE_Yes(luaL_checkstring(L, 1)));
		return 1;
	}


	void ApolloAPIInit_Dialog() {
		Apollo_State::RequireFunction("DIALOG_ANNOY", Dialog_Annoy);
		Apollo_State::RequireFunction("DIALOG_YES", Dialog_Yes);
		Apollo_State::RequireNeil("API/Dialog.neil");
	}

}