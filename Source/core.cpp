// Lic:
// Apollo
// Core
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

#include <iostream>

#include <QuickString.hpp>

#include <Globals.hpp>
#include <core.hpp>
#include <Crash.hpp>
#include <States.hpp>

#include <AP_Lua_CPP.hpp>


namespace Tricky_Apollo {

	std::string MainScript() {
		static std::string sMainScript = "";
		if (sMainScript != "") return sMainScript;
		for (auto FE : JCRPackage.Entries()) {
			if (TrickyUnits::suffixed(FE.first,"/MAIN.NEIL")) {
				sMainScript = FE.second.Entry();
				return sMainScript;
			}
		}
		Crash("Script Main.Neil not found in any directory\nof the main package", "", "", AE_NoMainScript);
	}
}