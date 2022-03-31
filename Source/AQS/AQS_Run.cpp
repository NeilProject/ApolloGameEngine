#include "AQS_Run.hpp"
#include <QuickString.hpp>

using namespace std;
using namespace TrickyUnits;

namespace ApolloQuickScript {

	enum ChunkType{Chunk,StateCall,LibFunction};

	class True_AQS_Parameter { 
	public:
		True_AQS_Instruction* parent{ nullptr };
	};

	class True_AQS_Instruction {
	public:
		True_AQS_Chunk* parent{ nullptr };

	};

	class True_AQS_Chunk {
	public:
		True_AQS_State* parent{ nullptr };
		ChunkType Type{ ChunkType::Chunk };
		vector<AQS_Instruction> Instructions;
		AQS_LibFunc LibFunc;
	};

	void True_AQS_State::_Load(jcr6::JT_Dir* d, string script) {
		scriptfile = script;
	}

	AQS_State True_AQS_State::Load(jcr6::JT_Dir* d, std::string script) {
		auto ret = make_shared<True_AQS_State>();
		ret->Load(d, script);
		return ret;
	}

	void True_AQS_State::Register(std::string Chunk, AQS_LibFunc Func) {
		Chunks[Upper(Chunk)] = make_shared<True_AQS_Chunk>();
		Chunks[Upper(Chunk)]->parent = this;
		Chunks[Upper(Chunk)]->Type = ChunkType::LibFunction;
		Chunks[Upper(Chunk)]->LibFunc = Func;
	}
}
