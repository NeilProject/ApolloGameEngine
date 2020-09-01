// Lic:
// Apollo
// Kthura API
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
// Version: 20.09.01
// EndLic
// C++
#include <iostream>
#include <string>
#include <map> 

// lua
#include <AP_Lua_CPP.hpp>

// Kthura
#include <Kthura.hpp>
#include <Kthura_SDL_Driver.hpp>

// Tricky's Units
#include <QuickString.hpp>

// Apollo
#include <globals.hpp>
#include <States.hpp>

#define qAssert(condition,err) if (!condition) Crash(err,State,Apollo_State::TraceBack(State))

// I hope you didn't mind that I was not game to keep typing this over and over and over....
#define qVerify() 	\
	string Tag = Upper(Trim(luaL_checkstring(L, 1)));\
	string State = luaL_checkstring(L, 2);\
	int ID = luaL_checkinteger(L, 3); \
	qAssert(Maps.count(Tag), "There is no Kthura map on slot-tag \"" + Tag + "\""); \
	qAssert(Maps[Tag].ID() == ID, "Kthura map verification failed on slot-tag =\"" + Tag + "\"\n" + to_string(ID) + " != " + to_string(Maps[Tag].ID()));

#define qObjVerify() \
	qVerify(); \
	string Layer = Upper(luaL_checkstring(L, 4)); \
	qAssert(Maps[Tag].Layers.count(Layer), "No layer named \"" + Tag + "\ while searching for object"); \
	string gType = luaL_checkstring(L, 5); \
	KthuraObject* obj = NULL; \
	if (gType == "string") { \
		string objTag = luaL_checkstring(L, 6); \
		qAssert(Maps[Tag].Layers[Layer].HasTag(objTag), "No object tagged \"" + objTag + "\" in layer " + Layer + " in map " + Tag); \
		obj = Maps[Tag].Layers[Layer].TagMap(objTag); \
	} else if (gType == "number") { \
		int objID = luaL_checkinteger(L, 6); \
		qAssert(Maps[Tag].Layers[Layer].GetIDMap().count(objID), "No object with ID  " + to_string(objID) + " in layer " + Layer + " in map " + Tag); \
		obj = Maps[Tag].Layers[Layer].GetIDMap()[objID]; \
	} else { \
		Crash("Invalid Object Verification Requiest!",State,Apollo_State::TraceBack(State)); \
		return 0;\
	}

#define qCase(wannahave,value) } else if (ObjKey==wannahave) { ret=value
#define qStCs(wannahave) else if (ObjKey==wannahave)

namespace Tricky_Apollo {

	using namespace std;
	using namespace NSKthura;
	using namespace TrickyUnits;
	

	map<string, Kthura> Maps;
	string LastState;

	void Kthura_Panic(string Error) {
		string Trace = "";
		if (LastState == "") {
			LastState = "C++";
		} else {
			Trace = Apollo_State::TraceBack(LastState);
		}
		Crash("Kthura threw an error!\n" + Error, LastState, Trace);
	}

	static int Kthura_Load(lua_State* L) {
		static unsigned int counter = 0;
		string Tag = Upper(Trim(luaL_checkstring(L, 1)));
		string State = luaL_checkstring(L, 2);
		string Prefix = luaL_checkstring(L, 3);
		if(Prefix!="" && (!suffixed(Prefix,"/"))) Prefix+="/";
		if (!Tag[0]) {
			do {
				char NTag[10];
				sprintf_s(NTag, 8, "%08X", counter++);
				Tag = NTag;
			} while (Maps.count(Tag));			
		}
		Maps[Tag].TexDir = &JCRPackage;
		Maps[Tag].Load(JCRPackage, Prefix);
		cout << "Kthura map " << Prefix << " loaded onto slot \"" << Tag << "\"; Map ID: " << Maps[Tag].ID() << "\n";
		lua_pushstring(L,Tag.c_str());
		lua_pushinteger(L, Maps[Tag].ID());
		return 2;
	}

	static int Kthura_Check(lua_State* L) {
		string Tag = Upper(Trim(luaL_checkstring(L, 1)));
		string State = luaL_checkstring(L, 2);
		qAssert(Maps.count(Tag), "There is no Kthura map on slot-tag \"" + Tag + "\"");		
		lua_pushinteger(L, Maps[Tag].ID());
		return 1;
	}

	static int Kthura_Kill(lua_State* L) {
		string Tag = Upper(Trim(luaL_checkstring(L, 1)));
		string State = luaL_checkstring(L, 2);
		qAssert(Maps.count(Tag), "There is no Kthura map on slot-tag \"" + Tag + "\"");
		Maps[Tag].KillMap();
		Maps.erase(Tag);
		return 0;
	}

	static int Kthura_Verify(lua_State* L) {
		qVerify();
		return 0;
	}

	static int Kthura_CountObjects(lua_State* L) {
		qVerify();
		string Layer = Upper(luaL_checkstring(L, 4));
		if (!Maps[Tag].Layers.count(Layer)) Crash("Map " + Tag + " does not have a layer named \"" + Layer + "\"", State, Apollo_State::TraceBack(State));
		lua_pushinteger(L, Maps[Tag].Layers[Layer].Objects.size());
		return 1;
	}

	static int Kthura_EnumObjects(lua_State* L) {
		qVerify();
		string Layer = Upper(luaL_checkstring(L, 4));
		if (!Maps[Tag].Layers.count(Layer)) { Crash("Map " + Tag + " does not have a layer named \"" + Layer + "\"", State, Apollo_State::TraceBack(State)); return 0; }
		for (auto& obj : Maps[Tag].Layers[Layer].Objects) {
			lua_pushinteger(L,obj.ID());
		}
		return Maps[Tag].Layers[Layer].Objects.size();
	}

	static int Kthura_MapHasLayer(lua_State* L) {
		qVerify();
		string Layer = Upper(luaL_checkstring(L, 4));
		lua_pushboolean(L,Maps[Tag].Layers.count(Layer));
		return 1;
	}

	static int Kthura_Layers(lua_State* L) {
		qVerify();
		int ret = 0;
		for (auto IT : Maps[Tag].Layers) {
			lua_pushstring(L, IT.first.c_str());
			ret++;
		}
		return ret;
	}

	static int Kthura_VerifyObject(lua_State* L) {
		qObjVerify();
		lua_pushinteger(L, obj->ID());
		return 1;
	}

	static int Kthura_GetObjInt(lua_State* L) {
		qObjVerify();
		string ObjKey = Upper(luaL_checkstring(L, 7));
		int ret = 0;
		if (ObjKey == "ID") {
			ret = obj->ID();
			qCase("X", obj->X());
			qCase("Y", obj->Y());
			qCase("W", obj->w);
			qCase("H", obj->h);
			qCase("INSX", obj->insertx);
			qCase("INSY", obj->inserty);
			qCase("R", obj->R);
			qCase("G", obj->G);
			qCase("B", obj->B);
			qCase("ALPHA", obj->Alpha255()); // Alpha1000 is depecrated, and for now I ain't gonna add it here!!
			qCase("SCALEX", obj->ScaleX);
			qCase("SCALEY", obj->ScaleY);
			qCase("DOMINANCE", obj->Dominance());
			qCase("ROTATION", obj->RotationDegrees());
			qCase("ROTATIONDEGREES", obj->RotationDegrees());
			//qCase("ROTATIONRADIANS", obj->RotationRadians());
			qCase("ANIMSPEED", obj->AnimSpeed);
			qCase("ANIMFRAME", obj->AnimFrame);
		} else {
			Crash("Unknown Object Integer field: "+ObjKey, State, Apollo_State::TraceBack(State));
		}
		lua_pushinteger(L, ret);
		return 1;
	}

	static int Kthura_GetObjStr(lua_State* L) {
		qObjVerify();
		string ObjKey = Upper(luaL_checkstring(L, 7));
		string ret = "";
		if (ObjKey == "TEXTURE") {
			ret = obj->Texture;
			qCase("TAG", obj->Tag());
			qCase("KIND", obj->Kind());
			qCase("LABELS", obj->Labels());
		} else {
			Crash("Unknown Object String field: " + ObjKey, State, Apollo_State::TraceBack(State));
		}
		lua_pushstring(L, ret.c_str());
		return 1;
	}

	static int Kthura_GetObjBool(lua_State* L) {
		qObjVerify();
		string ObjKey = Upper(luaL_checkstring(L, 7));
		bool ret = 0;
		if (ObjKey == "IMPASSIBLE") {
			ret = obj->Impassible();
			qCase("FORCEPASSIBLE", obj->ForcePassible());
			qCase("VISIBLE", obj->Visible);
		} else {
			Crash("Unknown Object Boolean field: " + ObjKey, State, Apollo_State::TraceBack(State));
		}
		lua_pushboolean(L, ret);
		return 1;
	}

	static int Kthura_SetObjInt(lua_State* L) {
		qObjVerify();
		string ObjKey = Upper(luaL_checkstring(L, 7));
		int value = luaL_checkinteger(L, 8);
		if (false) {}
		qStCs("X") obj->X(value);
		qStCs("Y") obj->Y(value);
		qStCs("W") obj->w = value;
		qStCs("H") obj->h = value;
		qStCs("INSX") obj->insertx = value;
		qStCs("INSY") obj->inserty = value;
		qStCs("R") obj->R = value;
		qStCs("G") obj->G = value;
		qStCs("B") obj->B = value;
		qStCs("ALPHA") obj->Alpha255(value);
		qStCs("SCALEX") obj->ScaleX = value;
		qStCs("SCALEY") obj->ScaleY = value;
		qStCs("DOMINANCE") obj->Dominance(value);
		qStCs("ROTATION") obj->RotationDegrees(value);
		qStCs("ROTATIONDEGREES") obj->RotationDegrees(value);
		//qStCs("ROTATIONRADIANS") obj->RotationRadians(value);
		qStCs("ANIMSPEED") obj->AnimSpeed = value;
		qStCs("ANIMFRAME") obj->AnimFrame = value;
		// Booleans will be handled as INT
		qStCs("IMPASSIBLE") obj->Impassible(value!=0);
		qStCs("FORCEPASSIBLE") obj->ForcePassible(value != 0);
		qStCs("VISBLE") obj->Visible = value != 0;
		else {
			Crash("Could not write to Object Integer/Boolean field: " + ObjKey, State, Apollo_State::TraceBack(State));
		}
		return 0;
	}

	static int Kthura_SetObjStr(lua_State* L) {
		qObjVerify();
		string ObjKey = Upper(luaL_checkstring(L, 7));
		string value = luaL_checkstring(L, 8);
		if (false) {}
		qStCs("TEXTURE") obj->Texture = value;
		qStCs("TAG") obj->Tag(value);
		qStCs("KIND") Crash("Kthura Object Field 'Kind' is read-only!", State, Apollo_State::TraceBack(State));
		qStCs("LABELS") obj->Labels(value);
		else {
			Crash("Could not write to Object String field: " + ObjKey, State, Apollo_State::TraceBack(State));
		}
		return 0;
	}

	static int Kthura_Draw(lua_State* L) {
		qVerify();
		string Layer = Upper(luaL_checkstring(L, 4));
		int scx = luaL_checkinteger(L, 5);
		int scy = luaL_checkinteger(L, 6);
		int inx = luaL_checkinteger(L, 7);
		int iny = luaL_checkinteger(L, 8);
		if (!Maps[Tag].Layers.count(Layer)) {
			Crash("Cannot draw non-existent layer: " + Layer, State, Apollo_State::TraceBack(State));
			return 0;
		}
		KthuraDraw::DrawMap(Maps[Tag].Layers[Layer], scx, scy, inx, iny);
		return 0;
	}

	void ApolloAPIInit_Kthura() {
		Kthura::Panic = Kthura_Panic;
		Kthura_Draw_SDL_Driver::Init();
		Apollo_State::RequireFunction("AKTHURA_Load", Kthura_Load);
		Apollo_State::RequireFunction("AKTHURA_Check", Kthura_Check);
		Apollo_State::RequireFunction("AKTHURA_Verify", Kthura_Verify);
		Apollo_State::RequireFunction("AKTHURA_CountObjects", Kthura_CountObjects);
		Apollo_State::RequireFunction("AKTHURA_HasLayer", Kthura_MapHasLayer);
		Apollo_State::RequireFunction("AKTHURA_Layers", Kthura_Layers);
		Apollo_State::RequireFunction("AKTHURA_VerifyObject", Kthura_VerifyObject);
		Apollo_State::RequireFunction("AKTHURA_GetObjInt", Kthura_GetObjInt);
		Apollo_State::RequireFunction("AKTHURA_SetObjInt", Kthura_SetObjInt);
		Apollo_State::RequireFunction("AKTHURA_GetObjStr", Kthura_GetObjStr);
		Apollo_State::RequireFunction("AKTHURA_SetObjStr", Kthura_SetObjStr);
		Apollo_State::RequireFunction("AKTHURA_GetObjBool", Kthura_GetObjBool);
		Apollo_State::RequireFunction("AKTHURA_EnumObjects", Kthura_EnumObjects);
		Apollo_State::RequireFunction("AKTHURA_Draw", Kthura_Draw);
		Apollo_State::RequireNeil("API/Kthura.neil");
	}
}