// Lic:
// Source/AQS/AQS_Run.cpp
// Apollo Quick Script
// version: 22.07.18
// Copyright (C) 2022 Jeroen P. Broks
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
#define InApollo

#undef AQS_ParameterDebug


#include "AQS_Run.hpp"
#include <iostream>
#include <QuickString.hpp>
#include <TrickySTOI.hpp>

#ifdef InApollo
#include <AP_Lua_CPP.hpp>
#include <States.hpp>

// Direct link. No we don't always need headers.
namespace Tricky_Apollo {
	std::map<std::string, int>* xgInt(std::string bundle);
	std::map<std::string, std::string>* xgStr(std::string bundle);
	std::map<std::string, bool>* xgBoo(std::string bundle);
}

#endif
using namespace Tricky_Apollo;

#define qcrash(msg,stt,chnk,clld,ret) { AQS_Panic(msg, stt, chnk, clld); return ret; }
#define lcrash(msg,stt,chnk,clld) { AQS_Panic(msg, stt, chnk, clld); thereareerrors=true; return; }

using namespace std;
using namespace TrickyUnits;

namespace ApolloQuickScript {

	enum class ChunkType{Chunk,StateCall,LibFunction};
	enum class ParaType { Unknown = 0, String = 1, Int = 2, Label = 3 }; // Numbers are VERY important here, so I didn't want to take any chances this could possible go wrong

	class True_AQS_Parameter { 
	public:
		True_AQS_Instruction* parent{ nullptr };
		ParaType Type{ ParaType::Unknown };
		std::string sValue{ "" };
		int iValue{ 0 };
		long long Label{ 0 };
	};

	class True_AQS_Instruction {
	public:
		True_AQS_Chunk* parent{ nullptr };
		unsigned char instag{ 0 };
		vector<AQS_Parameter> Parameters;
	};

	class True_AQS_Chunk {
	public:
		True_AQS_State* parent{ nullptr };
		ChunkType Type{ ChunkType::Chunk };
		vector<AQS_Instruction> Instructions;
		AQS_LibFunc LibFunc;
		string State{ "" };
		string Func{ "" };
		void Parse(string ex, string Bundle, string& Type, string& svalue, int& ivalue, bool& bvalue) {
#ifdef InApollo
			if (prefixed(ex, "$$")) { auto gs = xgStr(Bundle); svalue = (*gs)[right(Upper(ex), ex.size() - 2)]; bvalue = svalue.size() > 0; ivalue = 0; Type = "String"; }
			else if (prefixed(Upper(ex), "$$")) { auto gs = xgStr(Bundle); svalue = (*gs)[right(Upper(ex), ex.size() - 1)]; bvalue = svalue.size() > 0; ivalue = 0; Type = "String"; }
			else if (prefixed(Upper(ex), "%%")) { auto gs = xgInt(Bundle); ivalue = (*gs)[right(Upper(ex), ex.size() - 2)]; svalue = to_string(ivalue); bvalue = ivalue; Type = "Int"; }
			else if (prefixed(Upper(ex), "%")) { auto gs = xgInt(Bundle); ivalue = (*gs)[right(Upper(ex), ex.size() - 1)]; svalue = to_string(ivalue); bvalue = ivalue; Type = "Int"; }
			else if (prefixed(Upper(ex), "&")) { 
				auto gs = xgBoo(Bundle); 
				bvalue = (*gs)[right(Upper(ex), ex.size() - 1)]; 
				ivalue = bvalue;
				svalue = to_string(ivalue); bvalue = ivalue; 
				Type = "Bool";
				//cout << "AQS>Check: " << right(ex, ex.size() - 1) << " >> " << bvalue << endl; // debug only!
			}
			else 
#endif
			{
				bvalue = ex.size();
				ivalue = 0;
				svalue = ex;
				Type = "String";
			}
		}
		void Parse(string ex, string& Type, string& svalue, int& ivalue, bool& bvalue) { Parse(ex, "_GV_", Type, svalue, ivalue, bvalue); }
		void Parse(AQS_Parameter p, string Bundle, string& Type, string& svalue, int& ivalue, bool& bvalue) {
			switch (p->Type) {
			case ParaType::Int:
				Type = "Int";
				ivalue = p->iValue;
				bvalue = p->iValue;
				svalue = to_string(p->iValue);
				break;
			case ParaType::String:
				Parse(p->sValue, Bundle, Type, svalue, ivalue, bvalue);
				break;
			default:
				qcrash("Uncheckable parameter type", "???", "???", "???");
			}
		}
		void Parse(AQS_Parameter ex, string& Type, string& svalue, int& ivalue, bool& bvalue) { Parse(ex, "_GV_", Type, svalue, ivalue, bvalue); }

		int GetInt(string ex, string Bundle="_GV_") {
#ifdef InApollo
			auto gi{ xgInt(Bundle) };
			if (prefixed(ex, "%%")) return (*gi)[right(ex, ex.size() - 1)];
			else if (prefixed(ex, "%")) return (*gi)[right(ex, ex.size() - 2)];
#endif
			return ToInt(ex);
		}
		int GetInt(AQS_Parameter p, string Bundle = "_GV_") {
			switch (p->Type) {
			case ParaType::Int:
				return p->iValue;
			case ParaType::String:
				return GetInt(p->sValue, Bundle);
			default:
				qcrash("Uncheckable int type", "???", "???", "???", 0);
			}
		}

		void Define(string key, string value, string Bundle = "_GV_") {			
#ifdef InApollo
			if (prefixed(key, "$$")) {
				(*xgStr(Bundle))[right(key, key.size() - 2)] = value; 
				return;
			} else if (prefixed(key, "$")) {
				(*xgStr(Bundle))[right(key, key.size() - 1)] = value;
				return;
			}
			if (prefixed(key, "&&")) {
				(*xgBoo(Bundle))[right(key, key.size() - 2)] = Upper(value)=="TRUE";
				return;
			} else if (prefixed(key, "&")) {
				(*xgBoo(Bundle))[right(key, key.size() - 1)] = Upper(value)=="TRUE";
				return;
			}
#endif
			qcrash("String definition failed", "???", ","",", "");
		}
		void Define(string key, int value, string Bundle = "_GV_") {
#ifdef InApollo
			if (prefixed(key, "$")) {
				Define(key, to_string(value), Bundle);
				return;
			}
			if (prefixed(key, "%%")) {
				(*xgInt(Bundle))[right(key, key.size() - 2)] = value;
				return;
			} else if (prefixed(key, "%")) {
				(*xgInt(Bundle))[right(key, key.size() - 1)] = value;
				return;
			}
			if (prefixed(key, "&&")) {
				(*xgBoo(Bundle))[right(key, key.size() - 2)] = value;
				return;
			} else if (prefixed(key, "&")) {
				(*xgBoo(Bundle))[right(key, key.size() - 1)] = value;
				return;
			}
#endif
			qcrash("Int definition failed", "???", ","",", "");
		}
		

	};




	bool True_AQS_State::OverwriteProtection{ true };

	void True_AQS_State::_Load(jcr6::JT_Dir* d, string script,string calledby) {		
		scriptfile = script;		
		auto b = d->B(script); if (b->ReadString() != "\rAQSC\x1a") lcrash(script + " is not a compiled AQS script!", "N/A", "N/A", calledby);
		auto bit = b->ReadByte(); if (bit != 8) lcrash("Only 8-bit instructions supported in this version of AQL", script, "N/A", calledby);
		if (b->ReadByte() != 255) lcrash("No config yet supported in AQL. (File supposed for a higher version?)", script, "N/A", calledby);
		while (!b->eof()) {
			auto tag{ b->ReadByte() };
			switch (tag) {
			case 1: {
				auto chname = b->ReadString();
				auto ch = make_shared<True_AQS_Chunk>();				
				ch->parent = this;
				ch->Type = ChunkType::StateCall;
				ch->State = b->ReadString();
				ch->Func = b->ReadString();
				if (ch->State == "*LOADEDFROM*") {
					ch->State = calledby;
					if (calledby == "C++") lcrash("LFCHUNK load error", script, chname, calledby);
				}
				Chunks[Upper(chname)] = ch;
			}
				  break;
			case 2: { 
				auto ch = make_shared<True_AQS_Chunk>();
				ch->parent = this;
				ch->Type = ChunkType::Chunk;
				auto chname{ b->ReadString() };
				auto inssize{ b->ReadLong64() };
				Chunks[Upper(chname)] = ch;
				if (inssize < 0) lcrash("Negative number for instrunction count received", script, chname, calledby);
				for (long long i = 0; i < inssize; ++i) {
					auto ins = make_shared<True_AQS_Instruction>();
					ch->Instructions.push_back(ins);
					ins->parent = ch.get();
					ins->instag = b->ReadByte();
					auto parasize{ b->ReadLong64() };
					for (long long j = 0; j < parasize; ++j) {
						auto para = make_shared<True_AQS_Parameter>();
						para->parent = ins.get();
						para->Type = (ParaType)b->ReadByte();
						switch (para->Type) {
						case ParaType::Int: para->iValue = b->ReadInt(); break;
						case ParaType::Label: para->Label = b->ReadLong64(); break;
						case ParaType::String: para->sValue = b->ReadString(); break;
						default: lcrash("Unknown parameter type " + to_string((int)para->Type), script, chname, calledby);
						}
						ins->Parameters.push_back(para);
					}
				}
			} break;
			case 255:
				return;
			}
		}		
	}

	void True_AQS_State::param(AQS_Parameter p, std::string CB) {
		if (MaxPara >= 255) qcrash("Too many parameters", scriptfile, "N/A", CB);
		LastCallParameters[(byte)(++MaxPara)] = make_shared<True_AQS_Parameter>();
		LastCallParameters[(byte)MaxPara]->sValue = p->sValue;
		LastCallParameters[(byte)MaxPara]->iValue = p->iValue;
		LastCallParameters[(byte)MaxPara]->Label = p->Label;
		LastCallParameters[(byte)MaxPara]->Type = p->Type;
#ifdef AQS_ParameterDebug
		cout << "Parameter pushed on position: " << MaxPara << endl;
#endif
	}



	AQS_State True_AQS_State::Load(jcr6::JT_Dir* d, std::string script, string calledby) {
		auto ret = make_shared<True_AQS_State>();
		if (d == nullptr) qcrash("JCR Resource is null", "N/A", "N/A", calledby, nullptr);
		if (!d->EntryExists(script)) qcrash(string("Script '" + script + "' does not exist"), "N/A", "N/A", calledby, nullptr);
		if (ret->thereareerrors) return nullptr; // Safety precaution!
		ret->_Load(d, script,calledby);
		return ret;
	}

	bool True_AQS_State::HasChunk(std::string ch) { return Chunks.count(Upper(ch)); }

	void True_AQS_State::Register(std::string Chunk, AQS_LibFunc Func) {
		Chunks[Upper(Chunk)] = make_shared<True_AQS_Chunk>();
		Chunks[Upper(Chunk)]->parent = this;
		Chunks[Upper(Chunk)]->Type = ChunkType::LibFunction;
		Chunks[Upper(Chunk)]->LibFunc = Func;
	}

	int True_AQS_State::CheckInteger(byte idx, bool autoconvert) {
		True_AQS_Parameter* p{ LastCallParameters[idx].get() };
		if (idx > MaxPara) {
			AQS_Panic("Parameter index " + to_string(idx) + " out of range (0-" + to_string(MaxPara), scriptfile, "", "");
			return 0;
		}
		switch (p->Type) {
		case ParaType::Unknown:
			AQS_Panic("Unknown parameter on index " + to_string(idx), scriptfile,"","");
			break;
		case ParaType::String:
			if (!autoconvert) AQS_Panic("String parameter on index " + to_string(idx)+"! Expected integer", scriptfile, "", "");
			return ToInt(p->sValue);
		case ParaType::Int:
			return p->iValue;
		default:
			AQS_Panic("Internal error! Parameter type defined in an unsupported type " + to_string((int)p->Type), scriptfile, "", "");
			return 0; // Security!
		}		
	}

	string True_AQS_State::CheckString(byte idx, bool autoconvert) {
		True_AQS_Parameter* p{ LastCallParameters[idx].get() };
		switch (p->Type) {
			if (idx > MaxPara) {
				AQS_Panic("Parameter index " + to_string(idx) + " out of range (0-" + to_string(MaxPara), scriptfile, "", "");
				return "";
			}
		case ParaType::Unknown:
			AQS_Panic("Unknown parameter on index " + to_string(idx), scriptfile, "", "");
			break;
			return string();
		case ParaType::String:
			return p->sValue;
		case ParaType::Int:
			if (!autoconvert) AQS_Panic("Integer parameter on index " + to_string(idx) + "! Expected string", scriptfile, "", "");
			return to_string(p->iValue);
		default:
			AQS_Panic("Internal error! Parameter type defined in an unsupported type " + to_string((int)p->Type), scriptfile, "", "");
			return ""; // Security!
		}
	}

	void True_AQS_State::CheckString(char* str, byte index, bool autoconvert) {
		auto r = CheckString(index, autoconvert);
		strcpy_s(str, sizeof(str), r.c_str());
	}

	void True_AQS_State::Call(std::string Chunk, std::string calledby) {
		Trans2Upper(Chunk);
		if (!Chunks.count(Chunk)) qcrash(string("Non-existent chunk (" + Chunk + ") called!"), scriptfile, Chunk, calledby);
		auto ch{ Chunks[Chunk].get() };
		switch (ch->Type) {
		case ChunkType::LibFunction:
			ch->LibFunc(this, calledby);
			return;
		case ChunkType::StateCall:
#ifdef InApollo
		{
			auto State{ ch->State };
			if (ch->State == "*CALLEDFROM*") State = calledby;
			if (!Apollo_State::HasState(State)) qcrash(string("Non-existent Apollo State (") + State + ")", scriptfile, Chunk, calledby);
			auto st = Apollo_State::Get(State);
			string param{ "" };
			//for (auto p : LastCallParameters) {
			for (int pi = 0; pi <= MaxPara; pi++) {
				auto p{ LastCallParameters[pi] };
#ifdef AQS_ParameterDebug
				cout << "About to call! Adding parameter #" << pi << "/" << MaxPara << ": " << p->sValue << endl;
#endif
				if (param.size()) param += ",";
				switch (p->Type) {
				case ParaType::Label:
					qcrash("Labels cannot be passed as parameters to function calls in Neil or Lua scripts", scriptfile, Chunk, calledby);
				case ParaType::Int:
					param += to_string(p->iValue);
					break;
				case ParaType::String: {
					auto s{ p->sValue };
					auto para = prefixed(s, "\"") && suffixed(s, "\"");
					for (int i = 0; i <= 127; i++) {
						//cout << "i = " << i << endl;
						if (i != 92 && i != 32 && (i < 34 && i > 123)) s = TReplace(s, (char)i, string("\\x") + right(string("0") + Hex(i), 2));
					}
					if (!para) param += '"';
					param += s;
					if (!para) param += '"';
				} break;
				default:
					qcrash("Unknown parameter kind", scriptfile, Chunk, calledby);
				}
			}
			if (param == "") param = "nil";
			//cout << "Calling to Neil: " << ch->Func << " ( " << param << " )\n";
			st->RawCallByType(ch->Func, param, 0);
			return;
		}
#else
			qcrash("AQS not tied to Apollo, so StateChunks cannot be called!", scriptfile, Chunk, calledby);
#endif		
		}
		size_t ipos = 0;
		auto insv = Chunks[Chunk]->Instructions;
		auto lastcheck{ false };
		auto start{ true };
		while (ipos < insv.size()) {
			auto ins = insv[ipos];
			switch (ins->instag) {
			case 0: // Exit
				return;
			case 1: // Call
			{
				ResetParams();
				auto fp{ ins->Parameters[0]->sValue };
				auto first{ true };
#ifdef AQS_ParameterDebug
				cout << "Prepare call!\n";
#endif
				for (auto cp : ins->Parameters) {
					if (!first) {
#ifdef AQS_ParameterDebug
						cout << "Call debug! >> " << (int)cp->Type << ": " << cp->sValue << endl; // Debug only!
#endif
						param(cp, calledby);
					}
					first = false;
				}
				// cout << "Call request: " << fp << endl; // debug only
				Call(fp, calledby);
				break;
			}
			case 2: // Jmp
				if (!ins->Parameters.size()) qcrash("Jump without jumpdata", scriptfile, Chunk, calledby);
				if (ins->Parameters[0]->Type != ParaType::Label) qcrash("Label expected for jump", scriptfile, Chunk, calledby);
				ipos = ins->Parameters[0]->Label;
				start = false;
				break;
			case 3:	{ // Chk
				if (!ins->Parameters.size()) qcrash("Check without jumpdata", scriptfile, Chunk, calledby);
				bool chk{ true };
				for (auto ex : ins->Parameters) {
					int i; string t, s; bool b;
					ch->Parse(ex, t, s, i, b);
					//cout << "Checking: " << ex->sValue << " --> " << b << endl;
					chk = chk && b;
					if (!chk) break;
				}
				lastcheck = chk;
			} break;
			case 4:
			case 5:
			case 6:	{
				int i1; string t1, s1; bool b1;
				int i2; string t2, s2; bool b2;
				if (!ins->Parameters.size()<2) qcrash("Not enough compare data", scriptfile, Chunk, calledby);
				ch->Parse(ins->Parameters[0], t1, s1, i1, b1);
				ch->Parse(ins->Parameters[1], t2, s2, i2, b2);
				if (t1 != t2) qcrash(string("Type mismatch (" + t1 + "!=" + t2 + ")"), scriptfile, Chunk, calledby);
				if (t1 != "Int" && ins->instag != 4) qcrash("Illegal compare", scriptfile, Chunk, calledby);
				switch (ins->instag) {
				case 4:
					if (t1 == "String")
						lastcheck = s1 == s2;
					else if (t1 == "Int")
						lastcheck = i1 == i2;
					else
						lastcheck = b1 == b2;
					break;
				case 5:
					lastcheck = i1 < i2;
					break;
				case 6:
					lastcheck = i1 > i2;
					break;
				}
			} break;
			case 7:
				if (!ins->Parameters.size()) qcrash("JumpTrue without jumpdata", scriptfile, Chunk, calledby);
				if (ins->Parameters[0]->Type != ParaType::Label) qcrash("Label expected for jump", scriptfile, Chunk, calledby);
				if (lastcheck) { ipos = ins->Parameters[0]->Label; start = false; }
				break;
			case 8:
				if (!ins->Parameters.size()) qcrash("JumpFalse without jumpdata", scriptfile, Chunk, calledby);
				if (ins->Parameters[0]->Type != ParaType::Label) qcrash("Label expected for jump", scriptfile, Chunk, calledby);
				if (!lastcheck) { ipos = ins->Parameters[0]->Label; start = false; }
				break;
			case 9:
				if (ins->Parameters.size() < 2) qcrash("MOV require more parameters", scriptfile, Chunk, calledby);
				switch (ins->Parameters[1]->Type) {
				case ParaType::String: ch->Define(ins->Parameters[0]->sValue, ins->Parameters[1]->sValue); break;
				case ParaType::Int:  ch->Define(ins->Parameters[0]->sValue, ins->Parameters[1]->iValue); break;
				default: qcrash("Invalid MOV value", scriptfile, Chunk, calledby);
				}
			case 10:
				if (ins->Parameters.size() < 1) qcrash("INC require more parameters", scriptfile, Chunk, calledby);
				ch->Define(ins->Parameters[0]->sValue, ch->GetInt(ins->Parameters[0]) + 1);
				break;
			case 11:
				if (ins->Parameters.size() < 1) qcrash("DEC require more parameters", scriptfile, Chunk, calledby);
				ch->Define(ins->Parameters[0]->sValue, ch->GetInt(ins->Parameters[0]) - 1);
				break;
			case 12:
			case 13: 
			case 14:
			case 15: {
				if (ins->Parameters.size() < 2) qcrash("ADD/SUB/MUL/DIV require more parameters", scriptfile, Chunk, calledby);
				auto
					a{ ch->GetInt(ins->Parameters[0]) },
					b{ ch->GetInt(ins->Parameters[1]) },
					c{ 0 };
				switch (ins->instag) {
				case 12: c = a + b; break;
				case 13: c = a - b; break;
				case 14: c = a * b; break;
				case 15: c = (int)floor(a / b); break;
				}
				ch->Define(ins->Parameters[0]->sValue, c);
			} break;
			case 16:	
			case 17:
			case 18:
			case 19: {
				if (ins->Parameters.size() < 3) qcrash("ADD2/SUB2/MUL2/DIV2 require more parameters", scriptfile, Chunk, calledby);
				auto
					a{ ch->GetInt(ins->Parameters[1]) },
					b{ ch->GetInt(ins->Parameters[2]) },
					c{ 0 };
				switch (ins->instag) {
				case 12: c = a + b; break;
				case 13: c = a - b; break;
				case 14: c = a * b; break;
				case 15: c = (int)floor(a / b); break;
				}
				ch->Define(ins->Parameters[0]->sValue, c);
			}break;
			case 20: {
				if (ins->Parameters.size() < 1) qcrash("DONE requires a parameter", scriptfile, Chunk, calledby);
				auto gs = xgBoo("_GV_");
				lastcheck = (*gs)[Upper(ins->Parameters[0]->sValue)];
				(*gs)[Upper(ins->Parameters[0]->sValue)] = true;
			} break;
			default:
				qcrash(string("Unknown instruction tag (" + to_string(ins->instag) + ")"), scriptfile, Chunk, calledby);				
			}
			if (start) ipos++; start = true;
		}
	}

	void True_AQS_State::ResetParams() { MaxPara = -1; }

	void True_AQS_State::ParamInt(int value,string CalledBy) {
		if (MaxPara >= 255) qcrash("Too many parameters", scriptfile, "N/A", CalledBy);
		LastCallParameters[(byte)(++MaxPara)] = make_shared<True_AQS_Parameter>();
		LastCallParameters[(byte)MaxPara]->iValue = value;
		LastCallParameters[(byte)MaxPara]->Type = ParaType::Int;		
	}

	void True_AQS_State::ParamString(string value, string CalledBy) {
		if (MaxPara >= 255) qcrash("Too many parameters", scriptfile, "N/A", CalledBy);
		LastCallParameters[(byte)(++MaxPara)] = make_shared<True_AQS_Parameter>();
		LastCallParameters[(byte)MaxPara]->sValue = value;
		LastCallParameters[(byte)MaxPara]->Type = ParaType::String;
	}

	void AQS_DefaultPanic(std::string msg, std::string AQS_State, std::string Chunk, std::string CalledBy) {
		cout << "AQS Panic: " << msg << "!\nAQS State: " << AQS_State << "; Chunk: " << Chunk;
		if (CalledBy != "") cout << "; Called by: " << CalledBy;
		cout << endl;
		exit(1000);
	}
	AQS_TPanic AQS_Panic{ AQS_DefaultPanic };
}