// Lic:
// Apollo
// Crash
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


// C++
#include <iostream>
#include <string>

// TQSG
#include <TQSG.hpp>

// Apollo
#include "../Headers/Crash.hpp"
#include "../Headers/SDL_Manager.hpp"

namespace Tricky_Apollo {
	using namespace std;
	using namespace TrickyUnits;

	void Crash(string Message, string State, string TraceBack, int exitcode) {
		cout << "\x1b[41;33;1mFatal Error!\x1b[0m\nError: " << Message << "\nState: " << State << "\n\n" << TraceBack << "\n";
		auto Tag = LoadTex("**DEATH**", "Pics/Death.png");
		
		TQSG_ClsColor(0, 0, 100);
		// do
		TQSG_Cls();
		TQMG_Color(0, 0, 255);
		Apollo_SDL_Draw("**DEATH**", 0, 0);
		TQMG_Color(0, 255, 255);
		Apollo_SysFont.Draw("Houston! We have a problem!", 5, 5);
		TQMG_Color(255, 255, 0);
		Apollo_SysFont.Draw(Message, 5, 30);
		int sy = 30 + Apollo_SysFont.TextHeight(Message.c_str());
		TQMG_Color(255, 255, 255);
		Apollo_SysFont.Draw(TraceBack,5,sy);
		sy += Apollo_SysFont.TextHeight(TraceBack.c_str());
		TQMG_Color(255, 0, 255);
		string h = "State: " + State;
		if (State == "") h += "C++";
		Apollo_SysFont.Draw(h, 5, sy);
		TQSG_Flip();
		// while(whatever)
		SDL_Delay(15000); // Temp function as I got no event manager yet!
		// loop
		ImmHalt(exitcode);
	}

	void ImmHalt(int exitcode) {
		Apollo_SDL_End();
		// TODO: Make sure Lua is unloaded
		exit(exitcode);
	}

}