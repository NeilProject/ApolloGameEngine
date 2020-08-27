package main


// This is only a lazy go on things.
// This go source is not needed to get Apollo itself on the run
// I merely used it to get q uick header of all APIs

// A header for each API was senseless to me... Each API only needed 
// one init function for the main application to call. Bit deal, huh?

import "fmt"
import "trickyunits/tree"
import "trickyunits/qstr"
import "trickyunits/qff"

const WorkDir = "/Projects/Applications/VisualStudio/VC/Apollo Game Engine/Source/API"

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
	qff.WriteStringToFile(WorkDir+"/ApolloAPIs.hpp",output)
	fmt.Print("All Done!")
}
