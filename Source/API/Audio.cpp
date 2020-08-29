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

	}
}