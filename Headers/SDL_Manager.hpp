// Lic:
// Apollo
// SDL Manager
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
// Version: 20.12.27
// EndLic


#ifndef INCLUDED_APOLLO_SDL_START_AND_FINISH
#define INCLUDED_APOLLO_SDL_START_AND_FINISH


#include <SDL.h>
#include <TQSG.hpp>
#include <TQSA.hpp>
#include <string>

namespace Tricky_Apollo {
	/// <summary>
	/// When set to true, all kind of debug crap will be shown on the console
	/// </summary>
	extern bool Apollo_SDL_Loudmouth;
	extern TrickyUnits::TQSG_ImageFont Apollo_SysFont;
	extern bool NeverFullScreen;
	
	
	

	// Texture
	TrickyUnits::TQSG_Image* GetTex(std::string Tag,std::string State="C++");
	bool HasTex(std::string Tag);
	void RemTex(std::string Tag);
	std::string LoadTex(std::string Tag, std::string File);
	std::string LoadAnimTex(std::string Tag, std::string File, int w, int h, int frames);
	void LoadTexture(std::string Tag, std::string File);	
	void LoadAnimTexture(std::string Tag, std::string File, int w, int h, int frames);
	int TexWidth(std::string Tag, std::string State="C++", std::string Traceback="");
	int TexHeight(std::string Tag, std::string State = "C++", std::string Traceback = "");
	void KillTex(std::string Tag); // RemTex requires a tag to exist, KillTex does not!
	bool TagExists(std::string Tag);

	// Fonts
	std::string LoadFont(std::string Tag, std::string File, std::string State = "C++");
	void LoadImageFont(std::string Tag, std::string File, std::string State = "C++");
	TrickyUnits::TQSG_ImageFont* APGetFont(std::string Tag, std::string State = "C++");
	bool GotFont(bool critical, std::string Tag, std::string State = "C++");
	void RemFont();
	void RemFont(std::string Tag);

	// Audio
	std::string Apollo_SDL_LoadAudio(std::string Tag, std::string File, std::string State = "C++", std::string Traceback = "");
	TrickyUnits::TQSA_Audio* Apollo_SDL_Audio(std::string Tag, std::string State = "C++");
	void Apollo_SDL_KillAudio(std::string Tag);
	

	// Base features
	void Apollo_SDL_Start();
	void Apollo_SDL_End();
	void Apollo_SDL_Flip();
	

	void Apollo_SDL_Draw(std::string Tag, int x, int y, std::string State = "C++", std::string Traceback = "No Traceback possible");
	void Apollo_SDL_Draw(std::string Tag, int x, int y, int f, std::string State = "C++", std::string Traceback = "No Traceback possible");
	void Apollo_SDL_Tile(std::string Tag, int x, int y, int w, int h,std::string State = "C++", std::string Traceback = "No Traceback possible");
	void Apollo_SDL_Tile(std::string Tag, int x, int y, int w, int h, int f, std::string State = "C++", std::string Traceback = "No Traceback possible");
	void Apollo_SDL_Stretch(std::string Tag, int x, int y, int w, int h, std::string State = "C++", std::string Traceback = "No Traceback possible");
	void Apollo_SDL_Stretch(std::string Tag, int x, int y, int w, int h, int f, std::string State = "C++", std::string Traceback = "No Traceback possible");
	//void Apollo_SDL_ClsColor(int r, int g, int b);
	//void Apollo_SDL_Cls();

}
#endif