// Lic:
// Apollo
// Audio API
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
// Version: 20.08.29
// EndLic
// Lua
#include <AP_Lua_CPP.hpp>


// Apollo
#include <SDL_Manager.hpp>
#include <States.hpp>

namespace Tricky_Apollo {
	static int AAUA_LoadAudio(lua_State* L) {
		auto Tag = luaL_checkstring(L, 1);
		auto File = luaL_checkstring(L, 2);
		auto State = luaL_checkstring(L,3);
		lua_pushstring(L,Apollo_SDL_LoadAudio(Tag,  File, State, Apollo_State::TraceBack(State)).c_str());
		return 1;
	}

	static int AAUA_Play(lua_State* L) {
		auto Tag = luaL_checkstring(L, 1);
		auto Channel = luaL_checkinteger(L, 2);
		auto Loops = luaL_checkinteger(L, 3);
		auto State = luaL_checkstring(L, 4);
		
		if (Channel >= 0) {
			Apollo_SDL_Audio(Tag, State)->ChPlay(Channel, Loops);
			lua_pushinteger(L, Channel);
		} else {
			auto Ch = Apollo_SDL_Audio(Tag, State)->Play(Loops);
			lua_pushinteger(L, Ch);
		}
		return 1;
	}

	static int AAUA_Stop(lua_State* L) {
		auto Channel = luaL_checkinteger(L, 1);
		//auto Tag = luaL_checkstring(L, 1);
		//auto State = luaL_checkstring(L, 3);
		Mix_HaltChannel(Channel);
		return 0;
	}



	void ApolloAPIInit_Audio() {
		Apollo_State::RequireFunction("AAUA_Load", AAUA_LoadAudio);
		Apollo_State::RequireFunction("AAUA_Play", AAUA_Play);
		Apollo_State::RequireFunction("AAUA_Stop", AAUA_Stop);
		Apollo_State::RequireNeil("API/Audio.neil");

	}
}