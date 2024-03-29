// Lic:
// Apollo
// Core
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
// Version: 22.04.30
// EndLic

#include <iostream>

#include <QuickString.hpp>
#include <Platform.hpp>
#include <MD5.hpp>
#include <TQSE.hpp>
#include <TQSG.hpp>
#include <OpenURL.hpp>

#include <Globals.hpp>
#include <core.hpp>
#include <Crash.hpp>
#include <States.hpp>
#include <Identify.hpp>

#include <AP_Lua_CPP.hpp>
#include <SDL_Manager.hpp>


namespace Tricky_Apollo {

	using namespace std;
	using namespace TrickyUnits;

	static string CurrentFlow = "APOLLO_MAIN";
	static bool KeepLooping = true;
	static int ExitCode = 0;

	static int TickStart = 0;
	static int Ticks = 0;
	static int Cycles = 0;
	static int FlowMinTicks = 0;

	static vector<string> StatesToBeKilled;

	static float FPS() {
		if (Ticks == 0) return 0; // Prevent division by zero!
		float avgFPS = (float)Cycles / ((float)Ticks / 1000.f);		
		if (avgFPS > 2000000) {
			avgFPS = 0;
		}
		return avgFPS;
	}

	static string sFPS() {
		char Ret[27];
		sprintf_s(Ret, 25, "FPS %15.2f", FPS());
		string TrueRet = Ret;
		return TrueRet;
	}


	string MainScript() {
		static std::string sMainScript = "";
		if (sMainScript != "") return sMainScript;
		for (auto FE : JCRPackage.Entries()) {
			// cout << "Looking for main script >> " << FE.first << " check:" << TrickyUnits::suffixed(FE.first, "/MAIN.NEIL") << "\n";
			if (TrickyUnits::suffixed(FE.first,"/MAIN.NEIL")) {
				sMainScript = FE.second.Entry();
				return sMainScript;
			}
		}
		Crash("Script Main.Neil not found in any directory\nof the main package", "C++: Apollo Core initiation","Check package: "+PackageMainFile, AE_NoMainScript);
	}




	void GoToFlow(string Flow,string State) {
		Flow = Upper(Flow);
		cout << (Flow != "APOLLO_MAIN") << (!prefixed(Flow, "FLOW_")) << ((Flow != "APOLLO_MAIN" && (!prefixed(Flow, "FLOW_")))) << "<FlowPrefixCheck\n";
		if (Flow != "APOLLO_MAIN" && (!prefixed(Flow,"FLOW_"))) CurrentFlow = "FLOW_" + Flow; else CurrentFlow = Flow;
		cout << "Changing Flow: " << CurrentFlow << "\n";
		if (!Apollo_State::HasState(CurrentFlow))
			Crash("Flow \""+Flow+"\" doesn't exist",State,Apollo_State::TraceBack(State));
	}

	string GetCurrentFlow() { return CurrentFlow; }

	// API
	static int APICORE_GoToFlow(lua_State* L) {
		string f = Upper(luaL_checkstring(L, 1));		
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

	static int APICORE_FPS(lua_State* L) {
		lua_pushnumber(L,FPS());		
		return 1;
	}

	static int APICORE_sFPS(lua_State* L) {
		lua_pushstring(L, sFPS().c_str());
		return 1;
	}

	static int APICORE_showFPS(lua_State* L) {
		int x = luaL_checkinteger(L, 1);
		int y = luaL_checkinteger(L, 2);
		Apollo_SysFont.Draw(sFPS(), x, y);
		return 0;
	}

	static int APICORE_MinTicks(lua_State* L) {
		FlowMinTicks = luaL_checkinteger(L, 1);
		return 0;
	}

	static int APICORE_GetMinTicks(lua_State* L) {
		lua_pushinteger(L,FlowMinTicks);
		return 1;
	}

	static int APICORE_LoadFlow(lua_State* L) {		
		string ToFlow = luaL_checkstring(L, 1);
		string Script = luaL_checkstring(L, 2);
		ToFlow = Upper(ToFlow);
		if (!prefixed(ToFlow, "FLOW_")) ToFlow = "FLOW_" + ToFlow;
		if (ExtractExt(Script)=="") Script+=".neil";
		Apollo_State::Load(ToFlow, JCRPackage, Script);
		return 0;
	}

	static int APICORE_KillFlow(lua_State* L) {
		string kFlow = luaL_checkstring(L, 1);
		if (kFlow != "APOLLO_MAIN" && (!prefixed(kFlow, "FLOW_"))) kFlow = "FLOW_" + kFlow;
		Apollo_State::Kill(kFlow);
		return 0;
	}

	static int APICORE_LoadState(lua_State* L) {
		string ToState = luaL_checkstring(L, 1);
		string Script = luaL_checkstring(L, 2);
		ToState = Upper(ToState);
		Apollo_State::Load(ToState, JCRPackage, Script);
		return 0;
	}

	static int APICORE_KillState(lua_State* L) {
		string kState = luaL_checkstring(L, 1);
		Apollo_State::Kill(kState);
		return 0;
	}

	static int APICORE_PlanToKill(lua_State* L) {
		string kState = luaL_checkstring(L, 1);
		StatesToBeKilled.push_back(kState);
		return 0;
	}

	static int APICORE_Call(lua_State* L) {
		string cState = luaL_checkstring(L, 1);
		string cFunc = luaL_checkstring(L, 2);
		string cPara = luaL_checkstring(L, 3);
		bool cWantRet = luaL_optinteger(L, 4,0);
		auto s = Apollo_State::Get(cState);
		// cout << "Calling: " << cState << "::" << cFunc << "( " << cPara << " )\n"; // DEBUG ONLY
		s->RawCallByType(cFunc, cPara);
		if (cWantRet) {
			/*
			for (int i = 1; i <= s->top(); ++i) {
				switch (s->ltype(i)) {
				case LUA_TNUMBER:
					lua_pushnumber(L, s->GetNum(i));
					break;
				case LUA_TBOOLEAN:
					lua_pushboolean(L, s->GetBool(i));
					break;
				case LUA_TSTRING:
					lua_pushstring(L, s->GetStr(i).c_str());
					break;
				case LUA_TNIL:
					lua_pushnil(L);
				default:
					Crash("Received value is of type " + s->stype(i) + ". Cannot process that type");
					break;
				}
			}
			return s->top();
			*/
		}
		return 0;
	}

	static int APICORE_HasState(lua_State* L) {
		string cState = luaL_checkstring(L, 1);
		lua_pushboolean(L,Apollo_State::HasState(cState));
		return 1;
	}

	static int APICORE_PKGDir(lua_State* L) {
		lua_pushstring(L, ExtractDir(PackageMainFile).c_str());
		return 1;
	}

	static int INTERSTATE_GetBool(lua_State* L) {
		string cState = luaL_checkstring(L, 1);
		string fState = luaL_checkstring(L, 2);
		string fVar = luaL_checkstring(L, 3);
		lua_pushboolean(L, Apollo_State::Get(fState, cState)->FetchBoolean(fVar));
		return 1;
	}

	static int INTERSTATE_GetInteger(lua_State* L) {
		string cState = luaL_checkstring(L, 1);
		string fState = luaL_checkstring(L, 2);
		string fVar = luaL_checkstring(L, 3);
		lua_pushinteger(L, Apollo_State::Get(fState, cState)->FetchInt(fVar));
		return 1;
	}

	static int INTERSTATE_GetString(lua_State* L) {
		string cState = luaL_checkstring(L, 1);
		string fState = luaL_checkstring(L, 2);
		string fVar = luaL_checkstring(L, 3);
		lua_pushstring(L, Apollo_State::Get(fState, cState)->FetchString(fVar).c_str());
		return 1;
	}

	static int INTERSTATE_SetString(lua_State* L) {
		string cState = luaL_checkstring(L, 1);
		string fState = luaL_checkstring(L, 2);
		string fVar = luaL_checkstring(L, 3);
		auto fVal = luaL_checkstring(L, 4);
		Apollo_State::Get(fState, cState)->DefString(fVar, fVal);
		return 0;
	}

	static int INTERSTATE_SetInt(lua_State* L) {
		string cState = luaL_checkstring(L, 1);
		string fState = luaL_checkstring(L, 2);
		string fVar = luaL_checkstring(L, 3);
		auto fVal = luaL_checkinteger(L, 4);
		Apollo_State::Get(fState, cState)->DefInt(fVar, fVal);
		return 0;
	}

	static int INTERSTATE_SetNumber(lua_State* L) {
		string cState = luaL_checkstring(L, 1);
		string fState = luaL_checkstring(L, 2);
		string fVar = luaL_checkstring(L, 3);
		auto fVal = luaL_checknumber(L, 4);
		Apollo_State::Get(fState, cState)->DefNumber(fVar, fVal);
		return 0;
	}

	static int INTERSTATE_SetBool(lua_State* L) {
		string cState = luaL_checkstring(L, 1);
		string fState = luaL_checkstring(L, 2);
		string fVar = luaL_checkstring(L, 3);
		auto fVal = luaL_checkstring(L, 4);
		Apollo_State::Get(fState, cState)->DefBool(fVar, fVal);
		return 0;
	}



	static int APICORE_Ticks(lua_State* L) {
		lua_pushinteger(L,(int)SDL_GetTicks());
		return 1;
	}

	static int APICORE_Shell(lua_State* L) {
		lua_pushinteger(L,system(luaL_checkstring(L, 1)));
		return 1;
	}

	static int APICORE_WaitMinTicks(lua_State* L) {
		static int old = 0;
		int CTicks = SDL_GetTicks();
		if (FlowMinTicks == 0) return 0;
		while (FlowMinTicks && old && (CTicks - old < FlowMinTicks) && old <= CTicks) {
			SDL_Delay(1);
			CTicks = SDL_GetTicks();
		}
		old = CTicks;
		return 0;
	}

	//static int APICORE_GetFlow(lua_State* L) {
	//	lua_pushstring(L, CurrentFlow.c_str());
	//	return 1;
	//}

	static int APICORE_Platform(lua_State* L) {
		bool g = luaL_optinteger(L, 1, 1);
		lua_pushstring(L, Platform(g).c_str());
		return 1;
	}

	static int APICORE_MD5(lua_State* L) {
		auto str = luaL_checkstring(L, 1);
		auto hash = qc_md5(str);
		lua_pushstring(L, hash.c_str());
		return 1;
	}

	static int APICORE_Sleep(lua_State* L) {
		auto
			tijd = luaL_checkinteger(L, 1);
		SDL_Delay(tijd);
		return 0;
	}

	static int APICORE_OpenURL(lua_State* L) {
		auto url{ luaL_checkstring(L,1) };
		TrickyUnits::OpenURL(url);
		return 0;
	}

	static int APICORE_aTan2(lua_State* L) {
		auto
			x{ luaL_checknumber(L,1) },
			y{ luaL_checknumber(L,2) };
		lua_pushnumber(L, atan2(x, y));
		return 1;
	}

	static int INTERSTATE_CheckStack(lua_State* L) {
		lua_pushstring(L, Apollo_State::StackDumpAll().c_str());
		return 1;
	}

	static int APICORE_Yes(lua_State* L) {
		auto question{ luaL_checkstring(L, 1) };
		lua_pushboolean(L, TQSE_Yes(question));
		return 1;
	}

	static int APICORE_Annoy(lua_State* L) {
		auto message{ luaL_checkstring(L, 1) };
		auto caption{ luaL_optstring(L,2,Identify::WindowTitle().c_str()) };
		auto pushed{ TQSE_AppTitle };
		TQSE_AppTitle = caption;
		TQSE_Notify(message);
		TQSE_AppTitle = pushed;
		return 0;		
	}

	static int APICORE_WinTitle(lua_State* L) {
		lua_pushstring(L, Identify::WindowTitle().c_str());
		return 1;
	}


	void InitCore() {
		Apollo_State::RequireFunction("GoToFlow",APICORE_GoToFlow);
		Apollo_State::RequireFunction("CurrentFlow", APICORE_GetFlow);
		Apollo_State::RequireFunction("ApolloQuit", APICORE_Quit);
		Apollo_State::RequireFunction("nFPS", APICORE_FPS);
		Apollo_State::RequireFunction("sFPS", APICORE_sFPS);
		Apollo_State::RequireFunction("showFPS", APICORE_showFPS);
		Apollo_State::RequireFunction("MinTicks", APICORE_MinTicks);
		Apollo_State::RequireFunction("GetMinTicks", APICORE_MinTicks);
		Apollo_State::RequireFunction("LoadFlow", APICORE_LoadFlow);
		Apollo_State::RequireFunction("KillFlow", APICORE_KillFlow);
		Apollo_State::RequireFunction("GetFlow", APICORE_GetFlow);
		Apollo_State::RequireFunction("MD5", APICORE_MD5);
		Apollo_State::RequireFunction("OpenURL", APICORE_OpenURL);
		Apollo_State::RequireFunction("aTan2", APICORE_aTan2);
		Apollo_State::RequireFunction("Yes", APICORE_Yes);
		Apollo_State::RequireFunction("Annoy", APICORE_Annoy);
		Apollo_State::RequireFunction("WinTitle", APICORE_WinTitle);

		Apollo_State::RequireFunction("CallState", APICORE_Call);
		Apollo_State::RequireFunction("LoadState", APICORE_LoadState);
		Apollo_State::RequireFunction("KillState", APICORE_KillState);
		Apollo_State::RequireFunction("PlanToKill", APICORE_PlanToKill);
		Apollo_State::RequireFunction("HasState", APICORE_HasState);
		Apollo_State::RequireFunction("PKGDir", APICORE_PKGDir);
		Apollo_State::RequireFunction("Ticks", APICORE_Ticks);
		Apollo_State::RequireFunction("Sleep", APICORE_Sleep);
		Apollo_State::RequireFunction("Shell", APICORE_Shell);
		Apollo_State::RequireFunction("Platform", APICORE_Platform);
		Apollo_State::RequireFunction("WaitMinTicks", APICORE_WaitMinTicks);
		Apollo_State::RequireFunction("INTERSTATE_GetInteger", INTERSTATE_GetInteger);
		Apollo_State::RequireFunction("INTERSTATE_GetString", INTERSTATE_GetString);
		Apollo_State::RequireFunction("INTERSTATE_GetBool", INTERSTATE_GetBool);
		Apollo_State::RequireFunction("INTERSTATE_SetInteger", INTERSTATE_SetInt);
		Apollo_State::RequireFunction("INTERSTATE_SetNumber", INTERSTATE_SetNumber);
		Apollo_State::RequireFunction("INTERSTATE_SetString", INTERSTATE_SetString);
		Apollo_State::RequireFunction("INTERSTATE_SetBool", INTERSTATE_SetBool);
		Apollo_State::RequireFunction("INTERSTATE_CheckStack", INTERSTATE_CheckStack);

		Apollo_State::RequireNeil("API/Core.Neil");
	}

	static void Apollo_CallBackHandler(SDL_Event* e) {
		Crash("The event callback routine has not yet been implemented!");
	}

	// Let's get ready to rumble!
	void RunTheGame() {
		static FlowType FT = Identify::GetFlowType();
		static int old = 0;
		TickStart = SDL_GetTicks();
		while (KeepLooping) {
			int CTicks = SDL_GetTicks();
			while (FlowMinTicks && old && (CTicks - old < FlowMinTicks) && old<=CTicks) {
				SDL_Delay(1); 
				CTicks = SDL_GetTicks();
			}
			old = CTicks;
			Ticks = CTicks - TickStart;
			Cycles++;			
			//cout << "Ticks = " << Ticks << ";\tTickStart = " << TickStart << ";\tCycles = " << Cycles << "; SDL_GetTicks() => " << SDL_GetTicks() << "; FPS() =>" << FPS() << "\n"; // debug
			auto* state = Apollo_State::Get(CurrentFlow);
			switch (FT) {
			case RegularRepeativeFlow:
				TQSG_Cls();
				TQSE_Poll();
				state->RawCallByType("Apollo_Flow","nil");
				TQSG_Flip();
				break;
			case SemiCallback:
				TQSG_Cls();
				state->RawCallByType("Apollo_Draw", "nil");
				TQSG_Flip();
				TQSE_Poll();
				state->RawCallByType("Apollo_Update", "nil");
				break;
			case FullCallback:
				TQSG_Cls();
				state->RawCallByType("Apollo_Draw", "nil");
				TQSG_Flip();
				TQSE_Poll(Apollo_CallBackHandler);
				state->RawCallByType("Apollo_Update", "nil");
				break;
			default:
				Crash("Unsupported flow type", "C++", "Identify", AE_UnknownFlowType);
				break;
			}
			for (auto s : StatesToBeKilled) {
				cout << "Executing planned kill: " << s << "\n";
				Apollo_State::Kill(s);
			}
			StatesToBeKilled.clear();
		}
		ImmHalt(ExitCode);
	}
}