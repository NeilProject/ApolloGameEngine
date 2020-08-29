// License Information:
// APIHeader.go
// APIHeader generator
// version: 20.08.29
// Copyright (C) 2020 Jeroen P. Broks
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
// End License Information
package main


// This is only a lazy go on things.
// This go source is not needed to get Apollo itself on the run
// I merely used it to get q uick header of all APIs

// A header for each API was senseless to me... Each API only needed 
// one init function for the main application to call. Bit deal, huh?

import "os"
import "fmt"
import "trickyunits/tree"
import "trickyunits/qstr"
import "trickyunits/qff"

const WorkDir = "/Projects/Applications/VisualStudio/VC/Apollo Game Engine/Source/API"
const ObjDir  = "/Projects/Applications/VisualStudio/VC/Apollo Game Engine/x64/Release"

func main() {
	boom:=tree.GetTree(WorkDir,false)
	output:="#pragma once\n\n// This is just a quickly generated header,\n// only used to make sure all APIs are properly linked to all states\n// It is however GPL3 licensed\n// And (c) Jeroen P. Broks\n\n\nnamespace Tricky_Apollo {\n";
	setinit:="\tvoid InitApolloAPIs() {\n\t\t// Refrain from saying this shouldn't be done in a header! Because I know!\n\n"
	for i,f:=range boom {
		if f=="ApolloAPIs.hpp" {
			fmt.Printf("%4d> Skipped: %s\n",i,f)
		} else {
			fmt.Printf("%4d> Working with: %s\n",i,f)
			InitFunction:="ApolloAPIInit_"+qstr.StripExt(f)
			output+=fmt.Sprintf("\tvoid %s();\n",InitFunction)
			setinit+=fmt.Sprintf("\t\t%s();\n",InitFunction)
		}		
	}
	setinit+="\t}\n"
	output+="\n\n\n"+setinit+"\n}"
	// fmt.Println(output) // Needed for testing, and besides, Go would crash otherwise!
	fmt.Println("Creating ApolloAPIs.hpp");
	qff.WriteStringToFile(WorkDir+"/ApolloAPIs.hpp",output)
	fmt.Println("Removing main.obj (would cause conflicts otherwise")
	err:=os.Remove(ObjDir+"/main.obj")
	if (err!=nil) {
		fmt.Println("\aRemoval failed!\t\t",err)
	}
	fmt.Println("All Done!")
}