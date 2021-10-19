// Lic:
// Apollo Game Engine
// Date & Time
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
// Version: 21.10.20
// EndLic
#include <States.hpp>
#include <TrickyTime.hpp>

using namespace TrickyUnits;

namespace Tricky_Apollo {

	static int ATIM_Date(lua_State* L) {
		auto date{ LocalTime() };
		lua_pushinteger(L, (lua_Integer)date.tm_mday);
		lua_pushinteger(L, (lua_Integer)date.tm_mon + 1);
		lua_pushinteger(L, (lua_Integer)date.tm_year + 1900);
		return 3;
	}

	static int ATIM_WeekDay(lua_State* L) {
		auto date{ LocalTime() };
		lua_pushinteger(L, (lua_Integer)date.tm_wday);
		return 1;
	}

	static int ATIM_Time(lua_State* L) {
		auto date{ LocalTime() };
		lua_pushinteger(L, (lua_Integer)date.tm_hour);
		lua_pushinteger(L, (lua_Integer)date.tm_min);
		lua_pushinteger(L, (lua_Integer)date.tm_sec);
		return 3;
	}



	void ApolloAPIInit_InitTime() {
		Apollo_State::RequireFunction("ATIM_DATE", ATIM_Date);
		Apollo_State::RequireFunction("ATIM_TIME", ATIM_Time);
		Apollo_State::RequireFunction("ATIM_WEEKDAY", ATIM_WeekDay);
		Apollo_State::RequireNeil("API/DateTime.neil");
	}
}