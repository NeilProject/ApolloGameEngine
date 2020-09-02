#pragma once

// This is just a quickly generated header,
// only used to make sure all APIs are properly linked to all states
// It is however GPL3 licensed
// And (c) Jeroen P. Broks


namespace Tricky_Apollo {
	void ApolloAPIInit_Audio();
	void ApolloAPIInit_Bank();
	void ApolloAPIInit_Events();
	void ApolloAPIInit_Graphics();
	void ApolloAPIInit_JCR6();
	void ApolloAPIInit_Kthura();



	void InitApolloAPIs() {
		// Refrain from saying this shouldn't be done in a header! Because I know!

		ApolloAPIInit_Audio();
		ApolloAPIInit_Bank();
		ApolloAPIInit_Events();
		ApolloAPIInit_Graphics();
		ApolloAPIInit_JCR6();
		ApolloAPIInit_Kthura();
	}

}