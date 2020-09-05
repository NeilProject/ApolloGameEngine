// Lic:
// Apollo
// Events
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
// Version: 20.09.05
// EndLic
#include <AP_Lua_CPP.hpp>
#include <TQSE.hpp>
#include <States.hpp>

namespace Tricky_Apollo {

	using namespace TrickyUnits;
	
	static int AEA_Poll(lua_State* L) {
		TQSE_Poll();
		return 0;
	}

	static int AEA_KeyDown(lua_State* L) {
		int i = luaL_checkinteger(L, 1);		
		auto r = TQSE_KeyDown((SDL_KeyCode)i);
		lua_pushboolean(L, r);
		return 1;
	}


	static int AEA_KeyHit(lua_State* L) {
		int i = luaL_checkinteger(L, 1);
		auto r = TQSE_KeyHit((SDL_KeyCode)i);
		lua_pushboolean(L, r);
		return 1;
	}

	static int AEA_Terminate(lua_State* L) {
		lua_pushboolean(L,TQSE_Quit());
		return 1;
	}

	static int AEA_MouseX(lua_State* L) {
		lua_pushinteger(L, TQSE_MouseX());
		return 1;
	}

	static int AEA_MouseY(lua_State* L) {
		lua_pushinteger(L, TQSE_MouseY());
		return 1;
	}

	static int AEA_HideMouse(lua_State* L) { HideMouse(); return 0; }
	static int AEA_ShowMouse(lua_State* L) { ShowMouse(); return 0; }



	void ApolloAPIInit_Events(){
		Apollo_State::RequireFunction("AEA_Poll", AEA_Poll);
		Apollo_State::RequireFunction("AEA_KeyDown", AEA_KeyDown);
		Apollo_State::RequireFunction("AEA_KeyHit", AEA_KeyHit);
		Apollo_State::RequireFunction("AEA_Terminate", AEA_Terminate);
		Apollo_State::RequireFunction("AEA_MouseX", AEA_MouseX);
		Apollo_State::RequireFunction("AEA_MouseY", AEA_MouseY);
		Apollo_State::RequireFunction("AEA_ShowMouse", AEA_ShowMouse);
		Apollo_State::RequireFunction("AEA_HideMouse", AEA_HideMouse);
		Apollo_State::RequireNeil("API/Events.neil");
	}
}