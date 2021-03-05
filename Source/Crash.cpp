// Lic:
// Apollo
// Crash
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
// Version: 21.03.05
// EndLic


// C++
#include <iostream>
#include <string>

// lua
#include <AP_Lua_CPP.hpp>

// TQSG
#include <TQSG.hpp>
#include <TQSE.hpp>

// Apollo
#include "../Headers/States.hpp"
#include "../Headers/Crash.hpp"
#include "../Headers/SDL_Manager.hpp"
#include "../Headers/Globals.hpp"

namespace Tricky_Apollo {
	using namespace std;
	using namespace TrickyUnits;

	void Crash(string Message, string State, string TraceBack, int exitcode) {
		static int CrashedBefore = false;
		cout << "\x1b[41;33;1mFatal Error!\x1b[0m\nError: " << Message << "\nState: " << State << "\n\n" << TraceBack << "\n";
		if (CrashedBefore) {
			ImmHalt(exitcode); return;
		}
		CrashedBefore = exitcode;
		//auto Tag = LoadTex("**DEATH**", "Pics/Death.png");
		//auto DW = TexWidth("**DEATH**");
		//auto DH = TexHeight("**DEATH**");
		auto
			Death = TQSG_LoadAutoImage(ARF, "Pics/Death.png");
		int
			DW = Death->W(),
			DH = Death->H(),
			SX, 
			SY,
			DX = 0, 
			DY = 0;
		TQSG_ScreenSize(&SX, &SY);
		if (DH < SY) DY = SY - DH;
		printf("DEATH %dx%d; Screen %dx%d; Coords(%d,%d)\n", DW, DH, SX, SY, DX, DY);
		TQSG_ClsColor(0, 0, 100);	
		SetScale(1, 1);
		do {
			TQSE_Poll();
			TQSG_Cls();
			TQSG_Color(0, 0, 255);
			//Apollo_SDL_Draw("**DEATH**", 0, DY);
			Death->Draw(DX, DY);
			TQSG_Color(0, 255, 255);
			Apollo_SysFont.Draw("Houston! We have a problem!", 5, 5);
			TQSG_Color(255, 255, 0);
			Apollo_SysFont.Draw(Message, 5, 30,0,0,TQSG_ScreenWidth()-10);
			int sy = 30 + (Apollo_SysFont.TextHeight(Message.c_str())*Apollo_SysFont.LastAutoNextCount);
			TQSG_Color(255, 255, 255);
			Apollo_SysFont.Draw(TraceBack, 5, sy);
			sy += Apollo_SysFont.TextHeight(TraceBack.c_str());
			TQSG_Color(255, 0, 255);
			string h = "State: " + State;
			if (State == "") h += "C++";
			Apollo_SysFont.Draw(h, 5, sy);

			TQSG_Color(255, 180, 0);
			Apollo_SysFont.Draw("Hit ESCAPE to exit", 5, TQSG_ScreenHeight()-30);
			TQSG_Flip();
		} while ((!TQSE_Quit()) && (!TQSE_KeyHit(SDLK_ESCAPE)));
		// SDL_Delay(15000); // Temp function as I got no event manager yet!
		// loop
		ImmHalt(exitcode);
	}

	void ImmHalt(int exitcode) {
		Maps.clear();
		if (NSKthura::KthuraDraw::DrawDriver) delete NSKthura::KthuraDraw::DrawDriver;
		if (NSKthura::Kthura::PathFinder) delete NSKthura::Kthura::PathFinder;
		Apollo_State::KillAll();
		Apollo_SDL_End();
		exit(exitcode);
	}

}