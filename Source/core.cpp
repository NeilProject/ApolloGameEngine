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
// Version: 20.08.26
// EndLic

#include <iostream>

#include <QuickString.hpp>

#include <Globals.hpp>
#include <core.hpp>
#include <Crash.hpp>
#include <States.hpp>

#include <AP_Lua_CPP.hpp>


namespace Tricky_Apollo {

	using namespace std;
	using namespace TrickyUnits;

	static string CurrentFlow = "APOLLO_MAIN";
	static bool KeepLooping = true;

	string MainScript() {
		static std::string sMainScript = "";
		if (sMainScript != "") return sMainScript;
		for (auto FE : JCRPackage.Entries()) {
			if (TrickyUnits::suffixed(FE.first,"/MAIN.NEIL")) {
				sMainScript = FE.second.Entry();
				return sMainScript;
			}
		}
		Crash("Script Main.Neil not found in any directory\nof the main package", "C++: Apollo Core initiation","Check package: "+PackageMainFile, AE_NoMainScript);
	}



	void RunTheGame() {

	}

	void GoToFlow(string Flow,string State) {
		Flow = Upper(Flow);
		if (Flow != "APOLLO_MAIN" && (!prefixed(Flow,"FLOW_"))) CurrentFlow = "FLOW_" + Flow; else CurrentFlow = Flow;
		if (!Apollo_State::HasState(CurrentFlow))
			Crash("Flow \""+Flow+"\" doesn't exist",State,Apollo_State::TraceBack(State));
	}

	string GetCurrentFlow() { return CurrentFlow; }

	// API
	int APICORE_GoToFlow(lua_State* L) {
		string f = luaL_checkstring(L, 1);
		GoToFlow(f, Apollo_State::NameFromState(L));
		return 0;
	}

	int APICORE_GetFlow(lua_State* L) {
		if (lua_gettop(L) != 0) cout << "\x1b[33mWARNING!\x1b[0m " << lua_gettop(L) << " parameter(s) given to GetFlow(). None are accepted! All are ignored!";
		lua_pushstring(L, CurrentFlow.c_str());
		return 1;
	}

	void InitCore() {
		Apollo_State::RequireFunction("GoToFlow",APICORE_GoToFlow);
		Apollo_State::RequireFunction("CurrentFlow", APICORE_GetFlow);
		Apollo_State::RequireNeil("API/Core.Neil");
	}


}