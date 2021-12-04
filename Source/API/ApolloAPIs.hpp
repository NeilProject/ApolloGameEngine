#pragma once

// This is just a quickly generated header,
// only used to make sure all APIs are properly linked to all states
// It is however GPL3 licensed
// And (c) Jeroen P. Broks


namespace Tricky_Apollo {
	void ApolloAPIInit_APIBank();
	void ApolloAPIInit_Audio();
	void ApolloAPIInit_Events();
	void ApolloAPIInit_Graphics();
	void ApolloAPIInit_JCR6();
	void ApolloAPIInit_Kthura();
	void ApolloAPIInit_RPGCharAPI();
	void ApolloAPIInit_Dialog();
	void ApolloAPIInit_InitGameVars();
	void ApolloAPIInit_InitTime();
	void ApolloAPIInit_Blop();
	void ApolloAPIInit_Medals();
	 


	void InitApolloAPIs() {
		// Refrain from saying this shouldn't be done in a header! Because I know!

		ApolloAPIInit_APIBank();
		ApolloAPIInit_Audio();
		ApolloAPIInit_Events();
		ApolloAPIInit_Graphics();
		ApolloAPIInit_JCR6();
		ApolloAPIInit_Kthura();
		ApolloAPIInit_RPGCharAPI(); 
		ApolloAPIInit_Dialog();
		ApolloAPIInit_InitGameVars();
		ApolloAPIInit_InitTime();
		ApolloAPIInit_Blop();
		ApolloAPIInit_Medals();
	}

}