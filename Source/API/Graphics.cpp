// Lic:
// Apollo
// Graphics API
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
// Version: 20.08.28
// EndLic
// We get into the deep of this later!

#include <AP_Lua_CPP.hpp>
#include <TQSG.hpp>
#include <States.hpp>

namespace Tricky_Apollo {
	using namespace TrickyUnits;

	// General graphics features
	static int AGGA_Cls(lua_State* L) {
		TQSG_Cls();
		return 0;
	}

	static int AGGA_ClsColor(lua_State* L) {
		int r = 0, g = 0, b = 0;
		r = luaL_checkinteger(L, 1);
		g = luaL_checkinteger(L, 2);
		b = luaL_checkinteger(L, 3);
		TQSG_ClsColor(r % 255, g % 255, b % 255);
		return 0;
	}

	static int AGGA_Color(lua_State* L) {
		int r = 0, g = 0, b = 0;
		r = luaL_checkinteger(L, 1);
		g = luaL_checkinteger(L, 2);
		b = luaL_checkinteger(L, 3);
		TQSG_Color(r % 255, g % 255, b % 255);
		return 0;
	}

	static int AGGA_GetColor(lua_State* L) {
		Uint8 r = 0, g = 0, b = 0;
		TQSG_GetColor(&r, &b, &b);
		lua_Integer rr = r, rg = g, rb = b;
		lua_pushinteger(L,rr);
		lua_pushinteger(L,rg);
		lua_pushinteger(L,rb);
		return 3;
	}

	static int AGGA_Alpha(lua_State* L) {
		if (lua_gettop(L)) TQSG_SetAlpha(luaL_checkinteger(L, 1));
		lua_pushinteger(L, TQSG_GetAlpha());
		return 1;
	}

	static int AGGA_Flip(lua_State* L) {
		TQSG_Flip;
		return 0;
	}

	static int AGGA_Rect(lua_State* L) {
		int x, y, w, h;
		bool open = false;
		x = luaL_checkinteger(L, 1);
		y = luaL_checkinteger(L, 2);
		w = luaL_checkinteger(L, 3);
		h = luaL_checkinteger(L, 4);
		if (lua_gettop(L) > 4) open = lua_toboolean(L, 5);
		TQSG_Rect(x, y, w, h, open);
		return 0;
	}

	static int AGGA_Circle(lua_State*L){
		int x, y, rad;
		x = luaL_checkinteger(L, 1);
		y = luaL_checkinteger(L, 2);
		rad = luaL_checkinteger(L, 3);
		TQSG_Circle(x, y, rad);
		return 0;
	}

	static int AGGA_Line(lua_State* L) {
		int x1, x2, y1, y2;
		x1 = luaL_checkinteger(L, 1);
		y1 = luaL_checkinteger(L, 2);
		x2 = luaL_checkinteger(L, 3);
		y2 = luaL_checkinteger(L, 4);
		TQSG_Line(x1, y1, x2, y2);
		return 0;
	}



	// Images

	// ImageFont


	void ApolloAPIInit_Graphics() {
		Apollo_State::RequireFunction("AGGA_Cls", AGGA_Cls);
		Apollo_State::RequireFunction("AGGA_ClsColor", AGGA_ClsColor);
		Apollo_State::RequireFunction("AGGA_Alpha", AGGA_Alpha); 
		Apollo_State::RequireFunction("AGGA_Flip", AGGA_Flip);
		Apollo_State::RequireFunction("AGGA_Rect", AGGA_Rect);
		Apollo_State::RequireFunction("AGGA_Circle", AGGA_Circle);
		Apollo_State::RequireFunction("AGGA_Line", AGGA_Line);
		Apollo_State::RequireNeil("API/Graphics.neil");
	}
}