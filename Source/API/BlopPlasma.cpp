// Lic:
// Apollo Game Engine
// Blop Plasma
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
// Version: 21.10.26
// EndLic
#include <AP_Lua_CPP.hpp>
#include <TQSG.hpp>
#include <TQSG_BlopPlasma.hpp>
#include <States.hpp>

using namespace TrickyUnits;

namespace Tricky_Apollo {

	static int Blop_New(lua_State* L) {
		auto
			NumberOfBlops{ luaL_optinteger(L,1,60) },
			Width{ luaL_optinteger(L,2,TQSG_ScreenWidth()) },
			Height{ luaL_optinteger(L,3,TQSG_ScreenHeight()) };
		NewBlopPlasma(NumberOfBlops, Width, Height);
		return 0;
	}

	static int Blop_Draw(lua_State* L) {
		auto NumBlops{ luaL_optinteger(L,1,0) };
		DrawBlopPlasma(NumBlops);
		return 0;
	}

	static int Blop_DrawCol(lua_State* L) {
		auto
			PlasR{ luaL_optnumber(L,1,1) },
			PlasG{ luaL_optnumber(L,2,1) },
			PlasB{ luaL_optnumber(L,3,1) };
		auto
			NumBlops{ luaL_optinteger(L,4,0) };
		DrawBlopPlasmaCol(PlasR, PlasG, PlasB, NumBlops);
		return 0;
	}

	void ApolloAPIInit_Blop() {
		//Apollo_State::RequireFunction("BLOP_New", Blop_New);
		Apollo_State::RequireFunction("BLOP_Create", Blop_New);
		Apollo_State::RequireFunction("BLOP_Draw", Blop_Draw);
		Apollo_State::RequireFunction("BLOP_DrawCol", Blop_Draw);
		Apollo_State::RequireNeil("API/Blop.Neil");


	}
}