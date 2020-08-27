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
// Version: 20.08.27
// EndLic

#include <iostream>

#include <QuickString.hpp>
#include <TQSE.hpp>

#include <Globals.hpp>
#include <core.hpp>
#include <Crash.hpp>
#include <States.hpp>
#include <Identify.hpp>

#include <AP_Lua_CPP.hpp>


namespace Tricky_Apollo {

	using namespace std;
	using namespace TrickyUnits;

	static string CurrentFlow = "APOLLO_MAIN";
	static bool KeepLooping = true;
	static int ExitCode = 0;

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




	void GoToFlow(string Flow,string State) {
		Flow = Upper(Flow);
		if (Flow != "APOLLO_MAIN" && (!prefixed(Flow,"FLOW_"))) CurrentFlow = "FLOW_" + Flow; else CurrentFlow = Flow;
		if (!Apollo_State::HasState(CurrentFlow))
			Crash("Flow \""+Flow+"\" doesn't exist",State,Apollo_State::TraceBack(State));
	}

	string GetCurrentFlow() { return CurrentFlow; }

	// API
	static int APICORE_GoToFlow(lua_State* L) {
		string f = luaL_checkstring(L, 1);
		GoToFlow(f, Apollo_State::NameFromState(L));
		return 0;
	}

	static int APICORE_GetFlow(lua_State* L) {
		if (lua_gettop(L) != 0) cout << "\x1b[33mWARNING!\x1b[0m " << lua_gettop(L) << " parameter(s) given to GetFlow(). None are accepted! All are ignored!";
		lua_pushstring(L, CurrentFlow.c_str());
		return 1;
	}

	static int APICORE_Quit(lua_State*L) {
		KeepLooping = false;
		if (lua_gettop(L) != 0) {
			ExitCode = luaL_checknumber(L, 1);
		}
		return 0;
	}

	void InitCore() {
		Apollo_State::RequireFunction("GoToFlow",APICORE_GoToFlow);
		Apollo_State::RequireFunction("CurrentFlow", APICORE_GetFlow);
		Apollo_State::RequireFunction("ApolloQuit", APICORE_Quit);
		Apollo_State::RequireNeil("API/Core.Neil");
	}

	// Let's get ready to rumble!
	void RunTheGame() {
		static FlowType FT = Identify::GetFlowType();
		while (KeepLooping) {
			auto* state = Apollo_State::Get(CurrentFlow);
			switch (FT) {
			case RegularRepeativeFlow:
				TQSE_Poll();
				state->RawCallByType("Apollo_Flow","nil");
				break;
			case SemiCallback:
				Crash("SemiCallback flow type not yet supported!");
				break;
			case FullCallback:
				Crash("Fullcallback flow type not yet supported!");
			default:
				Crash("Unsupported flow type", "C++", "Identify", AE_UnknownFlowType);
				break;
			}
		}
		ImmHalt(ExitCode);
	}
}