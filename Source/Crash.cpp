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
// Version: 20.08.24
// EndLic

#include "../Headers/Crash.hpp"
#include "../Headers/SDL_Manager.hpp"
#include <iostream>
#include <string>

namespace Tricky_Apollo {
	using namespace std;

	void Crash(string Message, string State, string TraceBack, int exitcode) {
		cout << "\x1b[41;33;1mFatal Error!\x1b[0m\nError: " << Message << "\nState: " << State << "\n\n" << TraceBack << "\n";
		auto Tag = LoadTex("**DEATH**", "Pics/Death.png");
		// loop
		ImmHalt(exitcode);
	}

	void ImmHalt(int exitcode) {
		Apollo_SDL_End();
		// TODO: Make sure Lua is unloaded
		exit(exitcode);
	}

}