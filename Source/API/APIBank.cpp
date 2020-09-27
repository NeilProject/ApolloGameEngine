// Lic:
// Apollo
// Bank
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
// Version: 20.09.27
// EndLic

// C++
#include <string>
#include <map>

// lua
#include <AP_Lua_CPP.hpp>

// Tricky's units
#include <Bank.hpp>
#include <QuickString.hpp>

// JCR6
#include <jcr6_core.hpp>

// Apollo
#include <Crash.hpp>
#include <States.hpp>
#include <Globals.hpp>

#define APoke(value)\
	string State = luaL_checkstring(L, 1); \
	string Tag = Upper(luaL_checkstring(L, 2)); \
	unsigned long long adres = luaL_checkinteger(L, 3); \
	value = luaL_checkinteger(L, 4); \
	if (!Banks.count(Tag)) Crash("Bank tagged \"" + Tag + "\" does not exist", State, Apollo_State::TraceBack(State)); \
	if (adres + sizeof(value) > Banks[Tag].Size()) Crash("Bank tagged \"" + Tag + "\" index out of range (" + to_string(adres) + ":" + to_string( Banks[Tag].Size())+")", State, Apollo_State::TraceBack(State)); \
	Banks[Tag].Poke(adres, value); \
	return 0;

#define APeek(sze)\
	string State = luaL_checkstring(L, 1);\
	string Tag = Upper(luaL_checkstring(L, 2));\
	unsigned long long Adres = luaL_checkinteger(L, 3);\
	if (!Banks.count(Tag)) Crash("Bank tagged \"" + Tag + "\" does not exist", State, Apollo_State::TraceBack(State)); \
	if (Adres + sze > Banks[Tag].Size()) Crash("Bank tagged \"" + Tag + "\" index out of range (" + to_string(Adres) + ":" + to_string( Banks[Tag].Size())+")", State, Apollo_State::TraceBack(State)); 


namespace Tricky_Apollo {

	using namespace std;
	using namespace TrickyUnits;

	map<string, TrBank> Banks;


	static int Apollo_BankCreate(lua_State *L){
		static int cUp = 0;
		string State = luaL_checkstring(L, 1);
		string Tag = Upper(luaL_checkstring(L, 2));
		int bytes = luaL_checkinteger(L, 3);
		if (Tag == "") {
			do {
				cUp++;
				Tag = "APOLLO_BANK:" + to_string(cUp);
			} while (Banks.count(Tag));
		}
		if (Banks.count(Tag)) Banks.erase(Tag);
		Banks[Tag].Create(bytes);
		lua_pushstring(L,Tag.c_str());
		return 1;
	}

	static int Apollo_BankKill(lua_State* L) {
		string State = luaL_checkstring(L, 1);
		string Tag = Upper(luaL_checkstring(L, 2));
		if (Banks.count(Tag)) Banks.erase(Tag);
		return 0;
	}

	static int Apollo_BankPokeChar(lua_State* L) {
		char c;
		APoke(c);
	}

	static int Apollo_BankPokeByte(lua_State* L) {
		unsigned char b;
		APoke(b);
	}

	static int Apollo_BankPokeInt32(lua_State* L) {
		int i;
		APoke(i);
	}

	static int Apollo_BankPokeUInt32(lua_State* L) {
		unsigned int ui;
		APoke(ui);
	}

	static int Apollo_BankPokeInt64(lua_State* L) {
		long long li;
		APoke(li);
	}

	static int Apollo_BankPokeUInt64(lua_State* L) {
		unsigned long long uli;
		APoke(uli);
	}

	static int Apollo_BankPeekByte(lua_State* L) {
		APeek(1);
		lua_pushinteger(L, Banks[Tag].PeekByte(Adres));
		return 1;
	}

	static int Apollo_BankPeekChar(lua_State* L) {
		APeek(1);
		lua_pushinteger(L, Banks[Tag].PeekChar(Adres));
		return 1;
	}

	static int Apollo_BankPeekInt32(lua_State* L) {
		APeek(sizeof(int));
		lua_pushinteger(L, Banks[Tag].PeekInt32(Adres));
		return 1;
	}

	static int Apollo_BankPeekInt64(lua_State* L) {
		APeek(sizeof(long long));
		lua_pushinteger(L, Banks[Tag].PeekInt64(Adres));
		return 1;
	}

	static int Apollo_BankPeekUInt32(lua_State* L) {
		APeek(sizeof(unsigned int));
		lua_pushinteger(L, Banks[Tag].PeekInt32(Adres));
		return 1;
	}

	static int Apollo_BankPeekUInt64(lua_State* L) {
		APeek(sizeof(unsigned long long));
		lua_pushinteger(L, Banks[Tag].PeekUInt64(Adres));
		return 1;
	}

	static int Apollo_LoadBank(lua_State* L) {
		using namespace jcr6;
		string State = luaL_checkstring(L, 1);
		string res = luaL_checkstring(L, 2);
		string Ent = luaL_checkstring(L, 3);
		JT_Dir AJD;
		JT_Dir* JD = NULL;
		if (res[0] == '*') {
			if (res == "*MPACKAGE") JD = &JCRPackage;
			else Crash("Unknown * tag sent to JCR6 API", State, Apollo_State::TraceBack(State));
		} else {
			AJD = Dir(res);
			JD = &AJD;
		}


		static int cUp = 0;
		string Tag = Upper(luaL_checkstring(L, 4));		
		if (Tag == "") {
			do {
				cUp++;
				Tag = "APOLLO_BANK:" + to_string(cUp);
			} while (Banks.count(Tag));
		}
		if (Banks.count(Tag)) Banks.erase(Tag);
		int bytes = JD->Entry(Ent).RealSize();
		Banks[Tag].Create(bytes);
		JT_EntryReader bt; 
		JD->B(Ent, bt);
		for (int p = 0; p < bt.getsize(); ++p) Banks[Tag].Poke(p, bt.ReadByte());
		lua_pushstring(L, Tag.c_str());
		//cout << "Returning from loadbank... Tag:" << Tag << "\n";
		return 1;
	}

	static int Apollo_BankCheck(lua_State* L) {
		string State = luaL_checkstring(L, 1); 
		string Tag = Upper(luaL_checkstring(L, 2)); 
		if (!Banks.count(Tag)) Crash("Bank tagged \"" + Tag + "\" does not exist", State, Apollo_State::TraceBack(State));
		return 0;
	}

	static int Apollo_BankSize(lua_State* L) {
		string State = luaL_checkstring(L, 1);
		string Tag = Upper(luaL_checkstring(L, 2));
		if (!Banks.count(Tag)) Crash("Bank tagged \"" + Tag + "\" does not exist", State, Apollo_State::TraceBack(State));
		lua_pushinteger(L,Banks[Tag].Size());
		return 1;
	}

	void ApolloAPIInit_Bank() {
		Apollo_State::RequireFunction("ABNK_Create", Apollo_BankCreate);
		Apollo_State::RequireFunction("ABNK_Kill", Apollo_BankKill);
		Apollo_State::RequireFunction("ABNK_PokeByte", Apollo_BankPokeByte);
		Apollo_State::RequireFunction("ABNK_PokeChar", Apollo_BankPokeChar);
		Apollo_State::RequireFunction("ABNK_PokeInt32", Apollo_BankPokeInt32);
		Apollo_State::RequireFunction("ABNK_PokeInt64", Apollo_BankPokeInt64);
		Apollo_State::RequireFunction("ABNK_PokeUInt32", Apollo_BankPokeUInt32);
		Apollo_State::RequireFunction("ABNK_PokeUInt64", Apollo_BankPokeUInt64);

		Apollo_State::RequireFunction("ABNK_PeekByte", Apollo_BankPeekByte);
		Apollo_State::RequireFunction("ABNK_PeekChar", Apollo_BankPeekChar);
		Apollo_State::RequireFunction("ABNK_PeekInt32", Apollo_BankPeekInt32);
		Apollo_State::RequireFunction("ABNK_PeekInt64", Apollo_BankPeekInt64);
		Apollo_State::RequireFunction("ABNK_PeekUInt32", Apollo_BankPeekUInt32);
		Apollo_State::RequireFunction("ABNK_PeekUInt64", Apollo_BankPeekUInt64);
		Apollo_State::RequireFunction("AJCR_LoadBank", Apollo_LoadBank);
		Apollo_State::RequireFunction("ABNK_BankCheck", Apollo_BankCheck);
		Apollo_State::RequireFunction("ABNK_BankSize", Apollo_BankSize);

		Apollo_State::RequireNeil("API/Bank.neil");
	}
}