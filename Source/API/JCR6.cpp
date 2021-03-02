// Lic:
// Source/API/JCR6.cpp
// Apollo
// version: 21.02.27
// Copyright (C) 2020, 2021 JCR6 access
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// EndLic
//C++
#include <string>

// Lua
#include <AP_Lua_CPP.hpp>

// JCR6
#include <jcr6_core.hpp>

// Units
#include <Dirry.hpp>
#include <QuickString.hpp>
#include <QuickStream.hpp>
#include <RPGSave.hpp>

// Apollo
#include <Crash.hpp>
#include <Globals.hpp>
#include <States.hpp>
#include <Identify.hpp>

namespace Tricky_Apollo {

	using namespace std;
	using namespace jcr6;
	using namespace TrickyUnits;

#define JCRCheck() \
	string res = luaL_checkstring(L, 1);\
	string State = luaL_checkstring(L, 2);\
	JT_Dir AJD;\
	JT_Dir* JD = NULL;\
	if (res[0] == '*') {\
		if (res == "*MPACKAGE") JD = &JCRPackage;\
		else Crash("Unknown * tag sent to JCR6 API", State, Apollo_State::TraceBack(State));\
	} else {\
		AJD = Dir(res);\
		JD = &AJD;\
	}


	/* Looking for trouble. In large complex project this will guarantee a crash
	static int Apollo_JCR6_Entries(lua_State* L) {
		JCRCheck()
		int c = 0;
		for (auto& e : JD->Entries()) {
			c++;
			cout << c << ":" << e.second.Entry() << "\n";
			lua_pushstring(L, e.second.Entry().c_str());
		}		
		return c;
	}
	*/


	// Better now to use, but if specific searches are not too big, safe. Deprecated though!
	static int Apollo_JCR6_SpecEntries(lua_State* L) {
		JCRCheck();
		string lookfor = Upper(luaL_checkstring(L, 3));
		int c = 0;
		for (auto& e : JD->Entries()) {
			if (prefixed(e.first, lookfor)) {
				c++;
				//cout << c << ":" << e.second.Entry() << "\n";
				lua_pushstring(L, e.second.Entry().c_str());
			}
		}
		return c;
	}

	vector<string> SEntries;
	static int Apollo_JCR6_StartEntries(lua_State* L) {
		JCRCheck();
		SEntries.clear();
		for (auto& e : JD->Entries()) {
			SEntries.push_back(e.second.Entry());
		}
		return 0;
	}
	static int Apollo_JCR_GetEntry(lua_State* L) {
		auto State = luaL_checkstring(L, 1);
		size_t idx = (size_t)luaL_checkinteger(L, 2); 
		if (idx<1 || idx>SEntries.size()) return 0;
		lua_pushstring(L, SEntries[idx - 1].c_str()); 
		return 1;
	}



	static int Apollo_JCR6_DirExists(lua_State* L) { 
		JCRCheck();
		string LookFor = Upper(luaL_checkstring(L, 3));
		for (auto& e : JD->Entries()) {
			if (prefixed(e.first, LookFor)) {
				lua_pushboolean(L, true); 
				return 1;
			}
		}
		lua_pushboolean(L, false);
		return 1;
	}

	

	static int Apollo_JCR6_LoadString(lua_State* L) {
		JCRCheck();
		string Entry = luaL_checkstring(L, 3);
		lua_pushstring(L, JD->String(Entry).c_str());
		return 1;
	}

	static int Apollo_JCR6_Size(lua_State* L) {
		JCRCheck();
		string Entry = luaL_checkstring(L, 3);
		lua_pushinteger(L, JD->Entry(Entry).RealSize());
		return 1;
	}

	static int Apollo_JCR_EntryExists(lua_State* L) {
		JCRCheck();
		string Entry = luaL_checkstring(L, 3);
		lua_pushboolean(L, JD->EntryExists(Entry));
		return 1;
	}

	// Savegames
	static map<string, string> SaveGameData;
	static string SaveGameFile{ "" };
	static string SaveDir{ "" };
	
	static int Apollo_SG_Assign(lua_State* L) {
		SaveGameFile = luaL_checkstring(L, 1);
		cout << "Savegame assigned to file: " << SaveGameFile << endl;
		return 0;
	}

	static int Apollo_SG_Clear(lua_State* L) {
		cout << "Cleared savegame!";
		SaveGameData.clear();
		return 0;
	}

	static int Apollo_SG_SetData(lua_State* L) {
		auto Key = Upper(luaL_checkstring(L, 1));
		SaveGameData[Key] = luaL_checkstring(L, 2);
		return 0;
	}

	static int Apollo_SG_GetData(lua_State* L) {
		auto Key = Upper(luaL_checkstring(L, 1));
		cout << "SG> Get: " << Key << endl;
		cout << "SG> Got: \"" << SaveGameData[Key] << "\"\n";
		lua_pushstring(L,SaveGameData[Key].c_str());
		return 1;
	}

	static int Apollo_SG_HasData(lua_State* L) {
		auto Key = Upper(luaL_checkstring(L, 1));
		lua_pushboolean(L, SaveGameData.count(Key));
		return 1;
	}

	static int Apollo_SG_DataFields(lua_State* L) {
		string ret{ "" };
		for (auto it : SaveGameData) {
			if (ret.size()) ret += ";";
			ret += it.first;
		}
		lua_pushstring(L, ret.c_str());
		return 1;
	}

	static int Apollo_SG_Save(lua_State* L) {
		cout << "Savegame write request received\n";
		if (SaveGameFile == "") Crash("Trying to save a savegame file without a name");
		if (SaveDir == "") SaveDir = Dirry("$Home$/ApolloGameData/"+Identify::ProjectData("ID")+"/SaveGame");
		string cmd{ "OpenURL MD \""+SaveDir+"\"" };
		cout << "chkexe: " << cmd << endl;
		cout << "Directory check: " << SaveDir << endl;
		auto exc = system(cmd.c_str());
		auto secu = false;
		if (exc) {
			lua_pushstring(L, std::string("Checking/Creating Savegame dir " + SaveDir + " failed! (" + to_string(exc) + ")").c_str()); // A more elegant solution may come later!
			return 1;
		}
		cout << "Saving: " << SaveDir << "/" << SaveGameFile << endl;
		jcr6::JT_Create JCR(SaveDir + "/"+ SaveGameFile);
		for (auto& it : SaveGameData) {
			if (it.first == "*PARTY") {
				cout << "= Adding party!";
				RPGSave(&JCR, "PARTY/");
				JCR.AddString("HEAD/PARTY", "Present");
			}else if(it.first=="*SECU") {
				secu = Upper(it.second) == "YES";
			} else if (prefixed(it.first,"*")){
				Crash("Invalid save game request!", "C++:SAVEGAME", it.first);
			} else {
				cout << "= Adding data entry: " << it.first << endl;
				if (it.first == "HEAD")
					JCR.AddString("HEAD/" + it.first, it.second);
				else
					JCR.AddString("DATA/" + it.first, it.second);
			}
		}
		if (secu) {
			string mksec{ "" };
			for (auto& it : SaveGameData) {
				if (!prefixed(it.first, "*")) {
					if (mksec.size()) mksec += "\n";
					mksec += hashmd5("DATA/"+it.first) + ":" + hashmd5(it.second);
				}
			}
			JCR.AddString("HEAD/SECU", mksec);
		}
		JCR.AddString("HEAD/ID.INI", "[SaveGame]\nID=" + Identify::ProjectData("ID") + "\nEngine=Apollo\nSub=GameCPSDL\nGameSig=" + Identify::ProjectData("SIG"));
		cout << "Finishing saving\n";
		JCR.Close();
		cout << "Saving complete\n";
		return 0;
	}

	static int Apollo_SG_Load(lua_State* L) {
		cout << "LoadGame write request received\n";
		if (SaveGameFile == "") Crash("Trying to load a savegame file without a name");
		if (SaveDir == "") SaveDir = Dirry("$Home$/ApolloGameData/" + Identify::ProjectData("ID") + "/SaveGame");
		string ff = SaveDir + "/" + SaveGameFile;
		if (!FileExists(ff)) Crash("Trying to load non-existent savegame file: " + ff);
		auto JCR = Dir(ff);		
		for (auto& ientry : JCR.Entries()) {
			auto ename = ientry.first;
			if (prefixed(ename, "DATA/")) {
				// TODO: cut dir and get data
				auto dname = right(ename, ename.size() - 5);
				SaveGameData[dname] = JCR.String(ename);
			} else if (ename == "HEAD/PARTY") {
				if (JCR.String("HEAD/PARTY") == "Present") {
					RPGLoad(&JCR, "PARTY/");
				}
			}
		}
		return 0;
	}

	static int Apollo_SG_LString(lua_State* L) {
		auto file = luaL_checkstring(L, 1);
		if (SaveDir == "") SaveDir = Dirry("$Home$/ApolloGameData/" + Identify::ProjectData("ID") + "/SaveGame");
		if (!FileExists(SaveDir+"/"+file)) return 0;
		lua_pushstring(L, LoadString(SaveDir+"/"+file).c_str());
		return 1; 
	}

	static int Apollo_SG_SString(lua_State* L) {
		auto
			file = luaL_checkstring(L, 1),
			str = luaL_checkstring(L, 2);
		if (SaveDir == "") SaveDir = Dirry("$Home$/ApolloGameData/" + Identify::ProjectData("ID") + "/SaveGame");
		SaveString(SaveDir + "/" + file, str);
		return 0;
	}

	static int Apollo_SG_Exists(lua_State* L) {
		string f = luaL_checkstring(L, 1);
		if (SaveDir == "") SaveDir = Dirry("$Home$/ApolloGameData/" + Identify::ProjectData("ID") + "/SaveGame");
		lua_pushboolean(L, FileExists(SaveDir+"/"+f));
		return 1;
	}

	static int Apollo_SG_Head(lua_State* L) {
		string ret = "";
		string f = luaL_checkstring(L, 1);
		if (SaveDir == "") SaveDir = Dirry("$Home$/ApolloGameData/" + Identify::ProjectData("ID") + "/SaveGame");
		string ff = SaveDir + "/" + f;
		bool bestrict = luaL_optinteger(L, 2, 0);
		if (!FileExists(ff)) {
			if (bestrict) Crash("Couldn't retreive the header of non-existent savegame: " + f);
			lua_pushstring(L, "");
			lua_pushstring(L, "File not found");
			return 2;
		}
		if (Recognize(ff) == "NONE") {
			if (bestrict) Crash("Savegame unrecognized: " + f);
			lua_pushstring(L, "");
			lua_pushstring(L, "Savegame unrecognized");
			return 2;
		}
		auto sg = Dir(ff);
		if (sg.EntryExists("HEAD/HEAD")) ret = sg.String("HEAD/HEAD");
		lua_pushstring(L, ret.c_str());
		return 1;
	}


	static int Apollo_ExtractDir(lua_State* L) { lua_pushstring(L, ExtractDir(luaL_checkstring(L, 1)).c_str()); return 1; }
	// Init

	void ApolloAPIInit_JCR6() {
		// JCR6 General
		//Apollo_State::RequireFunction("AJCR_Entries", Apollo_JCR6_Entries);
		Apollo_State::RequireFunction("AJCR_SpecEntries", Apollo_JCR6_SpecEntries);
		Apollo_State::RequireFunction("AJCR_LoadString", Apollo_JCR6_LoadString);
		Apollo_State::RequireFunction("AJCR_Size", Apollo_JCR6_Size);
		Apollo_State::RequireFunction("AJCR_EntryExists", Apollo_JCR_EntryExists);
		Apollo_State::RequireFunction("AJCR_DirExists", Apollo_JCR6_DirExists);
		Apollo_State::RequireFunction("AJCR_StartEntries", Apollo_JCR6_StartEntries);
		Apollo_State::RequireFunction("AJCR_GetEntry", Apollo_JCR_GetEntry);
		Apollo_State::RequireFunction("AJCR_ExtractDir", Apollo_ExtractDir);
		Apollo_State::RequireNeil("API/JCR6.neil");

		// SaveGames
		Apollo_State::RequireFunction("ASGM_Assign", Apollo_SG_Assign);
		Apollo_State::RequireFunction("ASGM_Clear", Apollo_SG_Clear);
		Apollo_State::RequireFunction("ASGM_SetData", Apollo_SG_SetData);
		Apollo_State::RequireFunction("ASGM_GetData", Apollo_SG_GetData);
		Apollo_State::RequireFunction("ASGM_HasData", Apollo_SG_HasData);
		Apollo_State::RequireFunction("ASGM_DataFields", Apollo_SG_DataFields);
		Apollo_State::RequireFunction("ASGM_Save", Apollo_SG_Save);
		Apollo_State::RequireFunction("ASGM_Load", Apollo_SG_Load);
		Apollo_State::RequireFunction("ASGM_Exists", Apollo_SG_Exists);
		Apollo_State::RequireFunction("ASGM_GetHeader", Apollo_SG_Head);
		Apollo_State::RequireFunction("ASGM_SaveIndieString", Apollo_SG_SString);
		Apollo_State::RequireFunction("ASGM_LoadIndieString", Apollo_SG_LString);
		Apollo_State::RequireNeil("API/SaveGame.neil");
	}
}