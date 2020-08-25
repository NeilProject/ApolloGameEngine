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
// Version: 20.08.25
// EndLic


#ifndef INCLUDED_APOLLO_SDL_START_AND_FINISH
#define INCLUDED_APOLLO_SDL_START_AND_FINISH


#include <SDL.h>
#include <TQSG.hpp>
#include <string>

namespace Tricky_Apollo {
	/// <summary>
	/// When set to true, all kind of debug crap will be shown on the console
	/// </summary>
	extern bool Apollo_SDL_Loudmouth;
	extern TrickyUnits::TQSG_ImageFont Apollo_SysFont;
	
	
	

	// Texture
	void RemTex(std::string Tag);

	// Base features
	void Apollo_SDL_Start();
	void Apollo_SDL_End();
	void Apollo_SDL_Flip();
	std::string LoadTex(std::string Tag, std::string File);
	int TexWidth(std::string Tag, std::string State="C++", std::string Traceback="");
	int TexHeight(std::string Tag, std::string State = "C++", std::string Traceback = "");

	void Apollo_SDL_Draw(std::string Tag, int x, int y, std::string State = "C++", std::string Traceback = "No Traceback possible");
	//void Apollo_SDL_ClsColor(int r, int g, int b);
	//void Apollo_SDL_Cls();

}
#endif