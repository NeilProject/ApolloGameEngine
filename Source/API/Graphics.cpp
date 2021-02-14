// Lic:
// Apollo
// Graphics API
// 
// 
// 
// (c) Jeroen P. Broks, 2020, 2021
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
// Version: 21.02.14
// EndLic
// We get into the deep of this later!

#include <AP_Lua_CPP.hpp>
#include <TQSG.hpp>
#include <SDL_Manager.hpp>
#include <States.hpp>
#include <string>
using namespace std;

namespace Tricky_Apollo {
	using namespace TrickyUnits;

	// Apollo General Graphics API
	static int AGGA_Cls(lua_State* L) {
		TQSG_Cls();
		return 0;
	}

	static int AGGA_ClsColor(lua_State* L) {
		int r = 0, g = 0, b = 0;
		r = luaL_checkinteger(L, 1);
		g = luaL_checkinteger(L, 2);
		b = luaL_checkinteger(L, 3);
		TQSG_ClsColor(r % 256, g % 256, b % 256);
		return 0;
	}

	static int AGGA_Color(lua_State* L) {
		int r = 0, g = 0, b = 0;
		r = luaL_checkinteger(L, 1);
		g = luaL_checkinteger(L, 2);
		b = luaL_checkinteger(L, 3);
		TQSG_Color(r % 256, g % 256, b % 256);
		return 0;
	}

	static int AGGA_Scale(lua_State* L) {
		//int w, h;
		//w = luaL_optinteger(L, 1, 1000);
		//h = luaL_optinteger(L, 2, w);
		double w, h;
		w = luaL_optnumber(L, 1, 1);
		h = luaL_optnumber(L, 2, w);
		SetScale(w, h);
		return 0; 
	}

	static int AGGA_GetColor(lua_State* L) {
		Uint8 r = 0, g = 0, b = 0;
		TQSG_GetColor(&r, &g, &b);
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
		TQSG_Flip();
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

	static int AGGA_ScreenW(lua_State* L) {
		lua_pushinteger(L,TQSG_ScreenWidth());
		return 1;
	}

	static int AGGA_ScreenH(lua_State* L) {
		lua_pushinteger(L, TQSG_ScreenHeight());
		return 1;
	}

	static int AGGA_SetAlpha(lua_State* L) {
		int a = luaL_checkinteger(L, 1);
		TQSG_SetAlpha(a);
		return 0;
	}

	static int AGGA_GetAlpha(lua_State* L) {
		lua_pushinteger(L, TQSG_GetAlpha());
		return 1;
	}



	// Apollo IMaGes
	static int AIMG_Load(lua_State* L) {
		std::string File = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		auto ret = LoadTex(Tag, File);
		lua_pushstring(L, ret.c_str());
		return 1;
	}

	static int AIMG_LoadNew(lua_State* L) {
		auto
			File = luaL_checkstring(L, 1),
			Tag = luaL_checkstring(L, 2);
		if (HasTex(Tag)) {
			lua_pushstring(L, Tag);
		} else {
			auto ret = LoadTex(Tag, File);
			lua_pushstring(L, ret.c_str());
		}
		return 1;
	}

	static int AIMG_LoadAnim(lua_State* L) {
		std::string File = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		int w = luaL_checkinteger(L, 3);
		int h = luaL_checkinteger(L, 4);
		int f = luaL_checkinteger(L, 5);
		auto ret = LoadAnimTex(Tag, File, w, h, f);
		lua_pushstring(L, ret.c_str());
		return 1;
	}

	static int AIMG_TagExists(lua_State* L) {
		std::string Tag = luaL_checkstring(L, 1);
		lua_pushboolean(L, TagExists(Tag));
		return 1;
	}



	static int AIMG_Kill(lua_State* L) {
		std::string Tag = luaL_checkstring(L, 1);
		KillTex(Tag);
		return 0;
	}

	static int AIMG_Draw(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		int x = floor(luaL_checknumber(L, 3));
		int y = floor(luaL_checknumber(L, 4));
		int f = luaL_checknumber(L, 5);
		//GetTex(Tag, State)->Draw(x, y, f);
		Apollo_SDL_Draw(Tag, x, y, f,State,Apollo_State::TraceBack(State));
		return 0;
	}

	static int AIMG_XDraw(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		int x = floor(luaL_checknumber(L, 3));
		int y = floor(luaL_checknumber(L, 4));
		int f = luaL_checknumber(L, 5);
		//GetTex(Tag, State)->Draw(x, y, f);
		Apollo_SDL_XDraw(Tag, x, y, f, State, Apollo_State::TraceBack(State));
		return 0;
	}

	static int AIMG_Tile(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		int x = floor(luaL_checknumber(L, 3));
		int y = floor(luaL_checknumber(L, 4));
		int w = floor(luaL_checknumber(L, 5));
		int h = floor(luaL_checknumber(L, 6));
		int f = luaL_checknumber(L, 7);
		//GetTex(Tag, State)->Draw(x, y, f);
		// std::cout << "Tile(" << x << "," << y << "," << w << "," << h << "," << f << ");\n";
		Apollo_SDL_Tile(Tag, x, y, w, h, f, State, Apollo_State::TraceBack(State));
		if (TQSG_GetError() != "") Crash(TQSG_GetError(), State, Apollo_State::TraceBack(State));
		return 0;
	}

	static int AIMG_Stretch(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		int x = floor(luaL_checknumber(L, 3));
		int y = floor(luaL_checknumber(L, 4));
		int w = floor(luaL_checknumber(L, 5));
		int h = floor(luaL_checknumber(L, 6));
		int f = luaL_checknumber(L, 7);
		//GetTex(Tag, State)->Draw(x, y, f);
		// std::cout << "Tile(" << x << "," << y << "," << w << "," << h << "," << f << ");\n";
		Apollo_SDL_Stretch(Tag, x, y, w, h, f, State, Apollo_State::TraceBack(State));
		if (TQSG_GetError() != "") Crash(TQSG_GetError(), State, Apollo_State::TraceBack(State));
		return 0;
	}


	static int AIMG_ImgWidth(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		lua_pushinteger(L, GetTex(Tag)->Width());
		return 1;
	}

	static int AIMG_ImgHeight(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		lua_pushinteger(L, GetTex(Tag)->Height());
		return 1;
	}



	static int AIMG_Hot(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		int x = floor(luaL_checknumber(L, 3));
		int y = floor(luaL_checknumber(L, 4));
		GetTex(Tag, State)->Hot(x, y);
		return 0;
	}

	static int AIMG_HotCenter(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		GetTex(Tag, State)->HotCenter();
		return 0;
	}

	static int AIMG_HotBottomCenter(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		GetTex(Tag, State)->HotBottomCenter();
		return 0;
	}

	static int AFNT_LoadImageFont(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		std::string File = luaL_checkstring(L, 3);
		std::string ret = LoadFont(Tag, File, State);
		lua_pushstring(L,ret.c_str());
		return 1;
	}

	static int AFNT_DrawText(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		std::string Txt = luaL_checkstring(L,3);	
		int x = luaL_checkinteger(L, 4);
		int y = luaL_checkinteger(L, 5);
		int ah = luaL_checkinteger(L, 6);
		int av = luaL_checkinteger(L, 7);
		bool autonext = luaL_checkinteger(L, 8) != 0;
		APGetFont(Tag)->Draw(Txt, x, y, ah, av, autonext);
		return 0;
	}

	static int AFNT_AssertFont(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		GotFont(true, Tag, State);
		return 0;
	}

	static int AFNT_Fix(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		APGetFont(Tag,State)->Fixed = luaL_checkinteger(L, 3);
		return 0;
	}
	
	static int AFNT_GFix(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		lua_pushboolean(L, APGetFont(Tag, State)->Fixed);
		return 1;
	}

	static int AFNT_TxtW(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		std::string Txt = luaL_checkstring(L, 3);
		lua_pushinteger(L, APGetFont(Tag, State)->TextWidth(Txt.c_str()));
		return 1;
	}

	static int AFNT_TxtH(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		std::string Txt = luaL_checkstring(L, 3);
		lua_pushinteger(L, APGetFont(Tag, State)->TextHeight(Txt.c_str()));
		return 1;
	}

	static int AIMG_GrabScreen(lua_State* L) {
		std::string State = luaL_checkstring(L, 1);
		std::string Tag = luaL_checkstring(L, 2);
		auto I = TQSG_GrabScreen();
		auto Ret{ MkTex(Tag) };
		SetTex(Ret, I);
		lua_pushstring(L, Ret.c_str());
		return 1;
	}

	static int AIMG_Copy(lua_State* L) {
		auto
			State = luaL_checkstring(L, 1),
			Ori = luaL_checkstring(L, 2);
		auto
			Tar = MkTex(luaL_checkstring(L, 3));
		auto
			OriTex = GetTex(Ori, State);
		auto
			TarTex = TQSG_Copy(OriTex);
		SetTex(Tar, TarTex);
		lua_pushstring(L, Tar.c_str());
		return 1;
	}

	static int AIMG_Negative(lua_State* L) {
		auto
			State = luaL_checkstring(L, 1),
			Ori = luaL_checkstring(L, 2);
		auto
			Tar = MkTex(luaL_checkstring(L, 3));
		auto
			OriTex = GetTex(Ori, State);
		auto
			TarTex = TQSG_Negative(OriTex);
		SetTex(Tar, TarTex);
		lua_pushstring(L, Tar.c_str());
		return 1;
	}

	static int AIMG_SetRotation(lua_State*L){
		auto
			kind = luaL_checkinteger(L, 1);
		auto
			value = luaL_checknumber(L, 2);
		switch (kind) {
		case 0:
			// degrees
			TQSG_Rotate(value);
			break;
		case 1:
			// Radians
			TQSG_RotateRAD(luaL_checknumber(L, 2));
			break;
		case 2:
			TQSG_RotateGRAD(luaL_checknumber(L, 3));
			break;
		default:
			Crash("Unknown rotation kind (" + std::to_string(kind) + ")");
			break;
		}
		return 0;
	}

	static int AIMG_GetRotation(lua_State* L) {
		double value = 0;
		auto kind{ luaL_checkinteger(L,1) };
		switch (kind) {
		case 0:
			value = TQSG_Rotate(); break;
		case 1:
			value = TQSG_RotateRAD(); break;
		case 2:
			value = TQSG_RotateGRAD(); break;
		default:
			Crash("Unknown rotation kind (" + std::to_string(kind) + ")");
			break;
		}
		lua_pushnumber(L, value);
		return 1;
	}

	

	// ImageFont


	// Init
	void ApolloAPIInit_Graphics() {
		// General
		Apollo_State::RequireFunction("AGGA_Cls", AGGA_Cls);
		Apollo_State::RequireFunction("AGGA_ClsColor", AGGA_ClsColor);
		Apollo_State::RequireFunction("AGGA_Alpha", AGGA_Alpha); 
		Apollo_State::RequireFunction("AGGA_Flip", AGGA_Flip);
		Apollo_State::RequireFunction("AGGA_Rect", AGGA_Rect);
		Apollo_State::RequireFunction("AGGA_Circle", AGGA_Circle);
		Apollo_State::RequireFunction("AGGA_Line", AGGA_Line);
		Apollo_State::RequireFunction("AGGA_ScreenW", AGGA_ScreenW);
		Apollo_State::RequireFunction("AGGA_ScreenH", AGGA_ScreenH);
		Apollo_State::RequireFunction("AGGA_Color", AGGA_Color);
		Apollo_State::RequireFunction("AGGA_GetColor", AGGA_GetColor);
		Apollo_State::RequireFunction("AGGA_Scale", AGGA_Scale);
		Apollo_State::RequireFunction("AGGA_SetAlpha", AGGA_SetAlpha);
		Apollo_State::RequireFunction("AGGA_GetAlpha", AGGA_GetAlpha);
		// Images
		Apollo_State::RequireFunction("AIMG_Load", AIMG_Load);
		Apollo_State::RequireFunction("AIMG_LoadAnim", AIMG_LoadAnim);
		Apollo_State::RequireFunction("AIMG_Kill", AIMG_Kill);
		Apollo_State::RequireFunction("AIMG_Stretch", AIMG_Stretch);
		Apollo_State::RequireFunction("AIMG_Draw", AIMG_Draw);
		Apollo_State::RequireFunction("AIMG_XDraw", AIMG_XDraw);
		Apollo_State::RequireFunction("AIMG_Tile", AIMG_Tile);
		Apollo_State::RequireFunction("AIMG_Hot", AIMG_Hot);
		Apollo_State::RequireFunction("AIMG_HotCenter", AIMG_HotCenter);
		Apollo_State::RequireFunction("AIMG_HotBottomCenter", AIMG_HotBottomCenter);
		Apollo_State::RequireFunction("AIMG_ImgHeight", AIMG_ImgHeight);
		Apollo_State::RequireFunction("AIMG_ImgWidth", AIMG_ImgWidth);
		Apollo_State::RequireFunction("AIMG_TagExists", AIMG_TagExists);
		Apollo_State::RequireFunction("AIMG_LoadNew", AIMG_LoadNew);
		Apollo_State::RequireFunction("AIMG_GrabScreen", AIMG_GrabScreen);
		Apollo_State::RequireFunction("AIMG_Copy", AIMG_Copy);
		Apollo_State::RequireFunction("AIMG_Negative", AIMG_Negative);
		Apollo_State::RequireFunction("AIMG_GetRotation", AIMG_GetRotation);
		Apollo_State::RequireFunction("AIMG_SetRotation", AIMG_SetRotation);
		// Alias in General 
		Apollo_State::RequireFunction("AGGA_GetRotation", AIMG_GetRotation);
		Apollo_State::RequireFunction("AGGA_SetRotation", AIMG_SetRotation);
		// Fonts
		Apollo_State::RequireFunction("AFNT_LoadImageFont", AFNT_LoadImageFont);
		Apollo_State::RequireFunction("AFNT_DrawText", AFNT_DrawText);
		Apollo_State::RequireFunction("AFNT_AssertFont", AFNT_AssertFont);
		Apollo_State::RequireFunction("AFNT_Fix", AFNT_Fix);
		Apollo_State::RequireFunction("AFNT_GFix", AFNT_GFix);
		Apollo_State::RequireFunction("AFNT_TxtW", AFNT_TxtW);
		Apollo_State::RequireFunction("AFNT_TxtH", AFNT_TxtH);
		// Link Script
		Apollo_State::RequireNeil("API/Graphics.neil");
		// Image Font
	}
}