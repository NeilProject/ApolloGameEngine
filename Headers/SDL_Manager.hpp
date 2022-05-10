// Lic:
// Apollo
// SDL Manager
// 
// 
// 
// (c) Jeroen P. Broks, 2020, 2021, 2022
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
// Version: 22.05.10
// EndLic


//#ifndef INCLUDED_APOLLO_SDL_START_AND_FINISH
//#define INCLUDED_APOLLO_SDL_START_AND_FINISH
#pragma once


#include <SDL.h>
#include <TQSG.hpp>
#include <TQSA.hpp>
#include <string>

namespace Tricky_Apollo {
	using namespace TrickyUnits;
	/// <summary>
	/// When set to true, all kind of debug crap will be shown on the console
	/// </summary>
	extern bool Apollo_SDL_Loudmouth;
	extern TrickyUnits::TQSG_ImageFont Apollo_SysFont;
	extern bool NeverFullScreen;
	
	
	// AS
	void CreateAS(unsigned int w, unsigned int h, std::string Tag);
	bool HasAS(std::string Tag);
	TQSG_ASScreen GetAS(std::string Tag);
	

	// Texture
	TrickyUnits::TQSG_Image* GetTex(std::string Tag,std::string State="C++");
	TrickyUnits::TQSG_AutoImage GetATex(std::string Tag, std::string State = "C++");
	bool HasTex(std::string Tag);
	void RemTex(std::string Tag);
	std::string LoadTex(std::string Tag, std::string File);
	std::string LoadAnimTex(std::string Tag, std::string File, int w, int h, int frames);
	void LoadTexture(std::string Tag, std::string File);	
	void LoadAnimTexture(std::string Tag, std::string File, int w, int h, int frames);
	int TexWidth(std::string Tag, std::string State="C++", std::string Traceback="");
	int TexHeight(std::string Tag, std::string State = "C++", std::string Traceback = "");
	void KillTex(std::string Tag); // RemTex requires a tag to exist, KillTex does not!
	void KillTexByPrefix(std::string _prefix);
	int Tex_FrameCount(std::string Tag, std::string State, std::string Traceback);
	bool TagExists(std::string Tag);
	void SetTex(std::string Tag, TQSG_AutoImage Img);
	std::string MkTex(std::string Tag);

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
	bool Apollo_SDL_HasAudioTag(std::string Tag);
	void Apollo_SDL_AudioVolume100(int channel, int value, std::string State = "C++");
	void Apollo_SDL_AudioVolume(int channel, int value, std::string State = "C++");
	

	// Base features
	void Apollo_SDL_Start();
	void Apollo_SDL_End();
	void Apollo_SDL_Flip();
	

	void Apollo_SDL_Draw(std::string Tag, int x, int y, std::string State = "C++", std::string Traceback = "No Traceback possible");
	void Apollo_SDL_Draw(std::string Tag, int x, int y, int f, std::string State = "C++", std::string Traceback = "No Traceback possible");
	void Apollo_SDL_XDraw(std::string tag, int x, int y, int f, std::string State = "C++", std::string Traceback = "No Traceback possible");
	void Apollo_SDL_Tile(std::string Tag, int x, int y, int w, int h,std::string State = "C++", std::string Traceback = "No Traceback possible");
	void Apollo_SDL_Tile(std::string Tag, int x, int y, int w, int h, int f, std::string State = "C++", std::string Traceback = "No Traceback possible");
	void Apollo_SDL_Tile(std::string Tag, int x, int y, int w, int h, int ix, int iy, int f, std::string State, std::string Traceback="No Traceback possible");
	void Apollo_SDL_Stretch(std::string Tag, int x, int y, int w, int h, std::string State = "C++", std::string Traceback = "No Traceback possible");
	void Apollo_SDL_Stretch(std::string Tag, int x, int y, int w, int h, int f, std::string State = "C++", std::string Traceback = "No Traceback possible");
	//void Apollo_SDL_ClsColor(int r, int g, int b);
	//void Apollo_SDL_Cls();


#pragma region Header Alternate Screen System
	void Apollo_AS_Create(std::string Tag, unsigned int w, unsigned int h);
	TQSG_ASScreen Apollo_AS_Get(std::string Tag);
	bool Apollo_AS_Has(std::string Tag);
#pragma endregion

}
//#endif