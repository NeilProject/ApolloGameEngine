// Lic:
// Apollo Game Engine
// Game Engine
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
// Version: 21.07.18
// EndLic

// Internal
#include <iostream>
#include <stdio.h>

// Tricky's Units
#include <cpcd.h>
#include <ArgParse.h>
#include <QuickStream.hpp>
#include <QuickString.hpp>
#include <Dirry.hpp>
#include <TQSE.hpp>

// Lua

// SDL

// JCR6
#include <jcr6_core.hpp>
#include <jcr6_quakepak.hpp>
#include <jcr6_wad.hpp>
#include <jcr6_jxsda.hpp>
#include <jcr6_zlib.hpp>

// Apollo
#include <ErrorCodes.hpp>
#include <Globals.hpp>
#include <Identify.hpp>
#include <SDL_Manager.hpp>
#include <core.hpp>
#include <States.hpp>

// Init
#include "API/ApolloAPIs.hpp"

// Build
#include "../Headers/builddate.hpp"

using namespace TrickyUnits;
using namespace jcr6;

namespace Tricky_Apollo {
	void CheckARF() {
		cout << "\n\nStarting up JCR6\n";
		init_JCR6();
		init_quakepak();
		init_jxsda();
		init_zlib();
		InitWAD();
		string ARFile = StripExt(CLI_Config.myexe) + ".arf";
		if (!FileExists(ARFile)) { cout << "ERROR! " << ARFile << " not found!\n"; exit(AE_ARF_NotFound); }
		ARF = Dir(ARFile);
		auto ARFIDLines = ARF.Lines("ID/Identify.ini");
		auto E = Get_JCR_Error_Message();
		if (E != "Ok") {
			cout << "ERROR! No identification data or error in loading it in " << ARFile << "\nJCR6 reported: "<<E<<"\n"; exit(AE_ARF_Invalid);
		}
		GINIE AG; AG.Parse(ARFIDLines);
		if (AG.Value("ARF", "CHECK") != "Wendicka_Lovejoy") {
			cout << "ERROR! " << ARFile << " does not appear to be set up for the Apollo Game Engine!\n";
			exit(AE_ARF_Invalid);
		}
		cout << "Validated ARF:  " << ARFile << "\n";
		cout << "ARF build date: " << ARF.String("ID/Build")<<"\n\n";
	}

	void ApolloConfigFile() {
		GINIE RConfig;
		cout << "Apollo Core Config: " << Dirry("$AppSupport$/Apollo.ini") << endl;
		RConfig.FromFile(Dirry("$AppSupport$/Apollo.ini"));
		if (RConfig.Value("Apollo", "Home") != "")
			DirryVar("Home", RConfig.Value("Apollo", "Home"));		
	}

	void CLI_Args(int n, char* args[]) {
		FlagConfig Flag = {};
		AddFlag_Bool(Flag, "v", false); // Version		
		AddFlag_String(Flag, "h", Dirry("$Home$"));
		AddFlag_String(Flag, "sg", Dirry("$Save$"));
		AddFlag_Bool(Flag, "nfs", false); // If set no full screen ever!
		printf("Parsing %d parameter(s)\n", n);
		CLI_Config = ParseArg(n, args, Flag);
		if (ParseArgReport() != "") {
			cout << "Error occured: " << ParseArgReport() << "\n";
			exit(AE_CLI_Arg_Error);
		}
		cout << "Executable: " << CLI_Config.myexe << "\n";
		if (CLI_Config.string_flags["h"] != "") {
			cout << "Overriding home due to flag: " << CLI_Config.string_flags["h"] << endl;
			DirryVar("Home", CLI_Config.string_flags["h"]);
			DirryVar("Save", CLI_Config.string_flags["h"]);
		}
		if (CLI_Config.bool_flags["nfs"]) NeverFullScreen = true;
	}

	void FindGameData() {
		if (CLI_Config.arguments.size() > 0) {
			PackageMainFile = CLI_Config.arguments[0];
		}
		else {
			// Please note!! The line below only applies to Windows. In Linux and especially on Mac, a different approach will be desireable! Keep that in line if you want to port this!
			PackageMainFile = StripExt(CLI_Config.myexe) + ".Apollo.JCR";
		}
		if (!FileExists(PackageMainFile)) {
			cout << "No game package specified! Either specify it as a parameter, or have the package named \"" << PackageMainFile << "\"\n\n";
			exit(AE_NoPackage);
		}
		cout << "Game package: " << PackageMainFile << "\n";
		cd(ExtractDir(PackageMainFile).c_str());
		cout << "Work dir: " << ExtractDir(PackageMainFile) << "\n";
		cout << "Analyzing: " << PackageMainFile << "\n";
		JCRPackage = jcr6::Dir(PackageMainFile);
		if (Get_JCR_Error_Message() != "Ok") {
			cout << "ERROR while analying JCR package: " << Get_JCR_Error_Message() << "\n\n";
			exit(AE_JCR6Error);
		}
		if (!JCRPackage.EntryExists("ID/Identify.ini")) {
			cout << "ERROR! Package " << PackageMainFile << " does not contain a ID/Identify.ini entry! Either this is not an Apollo package, or the file may be corrupted!\n";
			exit(AE_NoIdentify);
		}
		cout << "Identifying\n";
		Identify::LoadIdentify();
		if (Identify::EngineData("Main") != "APOLLO") {
			cout << "ERROR! Package is for the " << Identify::EngineData("Main") << " engine!\n";
			exit(AE_IdentifyError);
		}
		if (Identify::EngineData("Sub") != "GAMECPSDL") {
			cout << "ERROR! Package is for the Apollo " << Identify::EngineData("sub") << " engine!\n";
			exit(AE_IdentifyError);
		}
		cout << "\n\n";
		cout << "Title:     " << Identify::MetaData("Title") << "\n";
		cout << "Author:    " << Identify::MetaData("Author") << "\n";
		cout << "Copyright: " << Identify::MetaData("Copyright") << "\n\n";
		cout << "Starting up window entitled \"" << Identify::WindowTitle() << "\"; ";
		if (Identify::FullScreen())
			cout << "Full Screen";
		else
			cout << Identify::WinWidth() << "x" << Identify::WinHeight();
		cout << "\n";
	}

	void LookUpMainScript() {
		//cout << "\n\nInit scripting engine\n";
		cout << "\nLooking up: Main.neil\n";
		auto ms = MainScript();
		cout << "\nFound: " <<ms << "\n";
		Apollo_State::Load("APOLLO_MAIN", JCRPackage, ms);
	}

	static void Panic4JCR(string err) {
		Crash(err, "C++", "JCR6 error!");
	}
	
}


using namespace Tricky_Apollo;

int main(int n, char* args[]) {
	printf("Apollo Game Engine\nWritten by Jeroen P. Broks\nBuild date: %s\n(c) Jeroen P. Broks\nReleased under the terms of the GPL3\n\n",Appollo_BuildDate);
	//TQSE_Init();  TQSE_ShowKeyNames(); return 0; // debug	
	ApolloConfigFile();
	CLI_Args(n, args);
	CheckARF();
	FindGameData();
	Apollo_SDL_Start();
	JCRPanic = Panic4JCR;
	States_Init();
	InitCore();
	InitApolloAPIs();
	LookUpMainScript();
	RunTheGame();
	Apollo_SDL_End();
	return AE_NoError;
}