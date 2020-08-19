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
// Version: 20.08.19
// EndLic

#include <iostream>
#include <stdio.h>
#include <ArgParse.h>
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

}

using namespace Tricky_Apollo;

int main(int n, char* args[]) {
	printf("Apollo Game Engine\nWritten by Jeroen P. Broks\nBuild date: %s\n(c) Jeroen P. Broks\nReleased under the terms of the GPL3\n\n",__DATE__);
	CLI_Args(n, args);
	return AE_NoError;
}
