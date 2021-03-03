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