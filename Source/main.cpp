// Lic:
// Apollo Game Engine
// Game Engine
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
// Version: 20.08.20
// EndLic

// Internal
#include <iostream>
#include <stdio.h>

// Tricky's Units
#include <ArgParse.h>
#include <QuickStream.hpp>
#include <QuickString.hpp>

// Lua

// SDL

// JCR6

// Apollo
#include <ErrorCodes.h>
#include <globals.h>

using namespace TrickyUnits;


namespace Tricky_Apollo {


	void CLI_Args(int n, char* args[]){
		FlagConfig Flag = {};
		AddFlag_Bool(Flag, "v", false); // Version		
		printf("Parsing %d parameter(s)\n", n);
		CLI_Config = ParseArg(n, args, Flag);
		if (ParseArgReport() != "") {
			cout << "Error occured: " << ParseArgReport() << "\n";
			exit(AE_CLI_Arg_Error);
		}
		cout << "Executable: " << CLI_Config.myexe << "\n";
	}

	void FindGameData(){
		if (CLI_Config.arguments.size() > 0) {
			PackageMainFile = CLI_Config.arguments[0];
		}
		else {
			// Please note!! The line below only applies to Windows. In Linux and especially on Mac, a different approach will be desireable! Keep that in line if you want to port this!
			PackageMainFile = StripExt(CLI_Config.myexe) + ".Apollo.JCR";
		}
		if (!FileExists(PackageMainFile)){
			cout << "No game package specified! Either specify it as a parameter, or have the package named \"" << PackageMainFile << "\"\n\n";
			exit(AE_NoPackage);
		}
		cout << "Game package: " << PackageMainFile << "\n";

	}

}

using namespace Tricky_Apollo;

int main(int n, char* args[]) {
	printf("Apollo Game Engine\nWritten by Jeroen P. Broks\nBuild date: %s\n(c) Jeroen P. Broks\nReleased under the terms of the GPL3\n\n",__DATE__);
	CLI_Args(n, args);
	FindGameData();
	return AE_NoError;
}