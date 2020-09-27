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
// Version: 20.09.27
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
#include <Kthura_Dijkstra.hpp>

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

#define qObjActor() \
		qObjVerify(); \
		qAssert((obj->Kind() == "Actor"), "Object \"" + Tag + "\" is not an actor!"); \
		KthuraActor* actor = (KthuraActor*)obj;

#define qCase(wannahave,value) } else if (ObjKey==wannahave) { ret=value
#define qStCs(wannahave) else if (ObjKey==wannahave)

namespace Tricky_Apollo {

	using namespace std;
	using namespace NSKthura;
	using namespace TrickyUnits;
	
	
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
		Maps[Tag].KillMap();
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

	static int Kthura_SpawnObjTag(lua_State* L) {
		qObjVerify();
		string actortag = luaL_checkstring(L, 7);
		//auto actor = KthuraActor::Spawn(Maps[Tag].Layer(Layer), obj);
		//actor->Tag(actortag);		
		cout << "Spawning actor '" << actortag << "' on spot '" << obj->Tag() << ".\n";
		Maps[Tag].Layer(Layer)->Spawn(obj, actortag);
		cout << "Remapping\n";
		Maps[Tag].Layer(Layer)->RemapTags();
		cout << "Final Checkup\n";
		cout << "<Tags>" << Maps[Tag].Layer(Layer)->TagList() << "</Tags>\n";
		for (auto &dbg : Maps[Tag].Layer(Layer)->Objects) { cout << "ObjCheck: " << dbg.ID() << ": " << dbg.Kind() << " " << dbg.Tag() << "!\n"; }
		return 0;
	}

	static int Kthura_SpawnSpot(lua_State* L) {
		qVerify();
		string Layer = luaL_checkstring(L, 4);
		auto x = luaL_checkinteger(L, 5);
		auto y = luaL_checkinteger(L, 6);
		string actortag = luaL_checkstring(L, 7);
		//auto actor = KthuraActor::Spawn(Maps[Tag].Layer(Layer), x,y);
		//actor->Tag(actortag);
		Maps[Tag].Layer(Layer)->Spawn(actortag, x, y);
		Maps[Tag].Layer(Layer)->RemapTags();
		return 0;
	}

	static int Kthura_WalkTo(lua_State* L) {
		qObjActor();
		auto x = luaL_checkinteger(L, 7);
		auto y = luaL_checkinteger(L, 8);
		auto real = luaL_optinteger(L, 9, 1);
		obj->WalkTo(x, y,real);
		return 0;
	}

	static int Kthura_WalkToObject(lua_State* L) {
		qObjActor();
		string objtag = luaL_checkstring(L, 7);
		obj->WalkTo(objtag);
		return 0;
	}

	static int Kthura_MoveTo(lua_State* L) {
		//qObjActor();
		string Tag = Upper(Trim(luaL_checkstring(L, 1)));
		string State = luaL_checkstring(L, 2);
		int ID = luaL_checkinteger(L, 3);
		qAssert(Maps.count(Tag), "There is no Kthura map on slot-tag \"" + Tag + "\"");
		qAssert(Maps[Tag].ID() == ID, "Kthura map verification failed on slot-tag =\"" + Tag + "\"\n" + to_string(ID) + " != " + to_string(Maps[Tag].ID()));
		string Layer = Upper(luaL_checkstring(L, 4));
		qAssert(Maps[Tag].Layers.count(Layer), "No layer named \"" + Tag + "\ while searching for object");
		string gType = luaL_checkstring(L, 5);
		KthuraObject* obj = NULL;
		if (gType == "string") {
			string objTag = luaL_checkstring(L, 6);
			qAssert(Maps[Tag].Layers[Layer].HasTag(objTag), "No object tagged \"" + objTag + "\" in layer " + Layer + " in map " + Tag);
			obj = Maps[Tag].Layers[Layer].TagMap(objTag);
		} else if (gType == "number") {
			int objID = luaL_checkinteger(L, 6);
			qAssert(Maps[Tag].Layers[Layer].GetIDMap().count(objID), "No object with ID  " + to_string(objID) + " in layer " + Layer + " in map " + Tag);
			obj = Maps[Tag].Layers[Layer].GetIDMap()[objID];
		} else {
			Crash("Invalid Object Verification Requiest!", State, Apollo_State::TraceBack(State));
			return 0;
		}
		qAssert((obj->Kind() == "Actor"), "Object \"" + Tag + "\" is not an actor!"); 
		//auto actor = obj;
		auto x = luaL_checkinteger(L, 7);
		auto y = luaL_checkinteger(L, 8);
		obj->MoveTo(x, y);
		return 0;
	}

	static int Kthura_MoveToObject(lua_State* L) {
		qObjActor();
		string objtag = luaL_checkstring(L, 7);
		//actor->MoveTo(objtag);
		obj->MoveTo(objtag);
		return 0;
	}


	static int Kthura_GetObjInt(lua_State* L) {
		qObjVerify();
		string ObjKey = Upper(luaL_checkstring(L, 7));
		int ret = 0;
		if (ObjKey == "ID") {
			ret = obj->ID();
			qCase("X", obj->X());
			qCase("Y", obj->Y());
			qCase("W", obj->W());
			qCase("H", obj->W());
			qCase("INSX", obj->insertx());
			qCase("INSY", obj->inserty());
			qCase("R", obj->R());
			qCase("G", obj->G());
			qCase("B", obj->B());
			qCase("ALPHA", obj->Alpha255()); // Alpha1000 is depecrated, and for now I ain't gonna add it here!!
			qCase("SCALEX", obj->ScaleX());
			qCase("SCALEY", obj->ScaleY());
			qCase("DOMINANCE", obj->Dominance());
			qCase("ROTATION", obj->RotationDegrees());
			qCase("ROTATIONDEGREES", obj->RotationDegrees());
			//qCase("ROTATIONRADIANS", obj->RotationRadians());
			qCase("ANIMSPEED", obj->AnimSpeed());
			qCase("ANIMFRAME", obj->AnimFrame());
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
			ret = obj->Texture();
			qCase("TAG", obj->Tag());
			qCase("KIND", obj->Kind());
			qCase("LABELS", obj->Labels());
			qCase("WIND", obj->Wind());
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
			qCase("VISIBLE", obj->Visible());
			qCase("NOTINMOTIONTHEN0", obj->NotInMotionThen0());
			qCase("NOTMOVINGTHEN0", obj->NotInMotionThen0());
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
		qStCs("W") obj->W( value);
		qStCs("H") obj->H ( value);
		qStCs("INSX") obj->insertx ( value);
		qStCs("INSY") obj->inserty ( value);
		qStCs("R") obj->R ( value);
		qStCs("G") obj->G ( value);
		qStCs("B") obj->B ( value);
		qStCs("ALPHA") obj->Alpha255(value);
		qStCs("SCALEX") obj->ScaleX ( value);
		qStCs("SCALEY") obj->ScaleY ( value);
		qStCs("DOMINANCE") obj->Dominance(value);
		qStCs("ROTATION") obj->RotationDegrees(value);
		qStCs("ROTATIONDEGREES") obj->RotationDegrees(value);
		//qStCs("ROTATIONRADIANS") obj->RotationRadians(value);
		qStCs("ANIMSPEED") obj->AnimSpeed ( value);
		qStCs("ANIMFRAME") obj->AnimFrame ( value);
		// Booleans will be handled as INT
		qStCs("IMPASSIBLE") obj->Impassible(value!=0);
		qStCs("FORCEPASSIBLE") obj->ForcePassible(value != 0);
		qStCs("VISBLE") obj->Visible(value != 0);
		qStCs("NOTINMOTIONTHEN0") obj->NotInMotionThen0(value != 0);
		qStCs("NOTMOVINGTHEN0") obj->NotInMotionThen0(value != 0);
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
		qStCs("TEXTURE") obj->Texture ( value);
		qStCs("TAG") obj->Tag(value);
		qStCs("KIND") Crash("Kthura Object Field 'Kind' is read-only!", State, Apollo_State::TraceBack(State));
		qStCs("LABELS") obj->Labels(value);
		qStCs("WIND") obj->Wind(value);
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

	static int Kthura_GetTags(lua_State* L) {
		qVerify();
		string Layer = Upper(luaL_checkstring(L, 4));
		if (!Maps[Tag].Layers.count(Layer)) {
			Crash("Cannot get the object tags from a non-existent layer: " + Layer, State, Apollo_State::TraceBack(State));
			return 0;
		}
		lua_pushstring(L, Maps[Tag].Layer(Layer)->TagList().c_str());
		return 1;		
	}

	static int Kthura_RemapAllLayers(lua_State* L) {
		qVerify();
		Maps[Tag].Remap();
	}

	static int Kthura_MapMetaFields(lua_State* L) {
		qVerify();
		std::string ret = "";
		for (auto& it : Maps[Tag].MetaData) {
			if (ret != "") ret += ";";
			ret += it.first;
		}
		lua_pushstring(L, ret.c_str());
		return 1;
	}

	static int Kthura_MapMeta(lua_State* L) {
		qVerify();
		std::string fld = luaL_checkstring(L, 4);
		if (!Maps[Tag].MetaData.count(fld)) {
			lua_pushstring(L, "");
		} else {
			lua_pushstring(L, Maps[Tag].MetaData[fld].c_str());
		}
		return 1;
	}

	static int Kthura_DumpBlockMap(lua_State* L) {
		qVerify();
		std::string Layer = luaL_checkstring(L, 4);
		lua_pushstring(L,Maps[Tag].Layer(Layer)->BlockMapStringDump().c_str());
		return 1;
	}

	static int Kthura_DumpDom(lua_State*L) {
		qVerify();
		std::string Layer = luaL_checkstring(L, 4);
		std::string ret = "";
		for (auto it : Maps[Tag].Layer(Layer)->_DomMap) {
			if (ret != "") ret += ";";
			ret += "[" + it.first + "] " + it.second->Kind() + " " + it.second->Tag()+" ("+to_string(it.second->X())+","+to_string(it.second->Y())+") "+to_string(it.second->W())+"x"+to_string(it.second->H())+"  ";
			if (it.second->Visible()) ret += "visible "; 
		}
		lua_pushstring(L, ret.c_str());
		return 1;
	}

	static int Kthura_AnyThingMoving(lua_State* L) {
		qVerify(); 
		std::string Layer = luaL_checkstring(L, 4);
		auto Lay = Maps[Tag].Layer(Layer);
		bool ret{ false };
		for (auto o : Lay->Objects) {
			if (o.EKind() == KthuraKind::Actor) ret = ret || o.Moving();
		}
		lua_pushboolean(L, ret);
		return 1;
	}

	static int Kthura_KillObject(lua_State* L) {
		qObjVerify();
		Maps[Tag].Layer(Layer)->Kill(obj);
		return 0;
	}

	static int Kthura_GetWind(lua_State* L) {
		qObjVerify();
		lua_pushstring(L, obj->Wind().c_str());
		return 1;
	}
	
	static int Kthura_SetWind(lua_State* L) {
		qObjVerify();
		obj->Wind(luaL_checkstring(L, 7));
		return 0;
	}


	void ApolloAPIInit_Kthura() {
		Kthura::Panic = Kthura_Panic;
		Kthura::PathFinder = new Kthura_Dijkstra();
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
		Apollo_State::RequireFunction("AKTHURA_GetTags", Kthura_GetTags); 
		Apollo_State::RequireFunction("AKTHURA_RemapAllLayers", Kthura_RemapAllLayers);
		Apollo_State::RequireFunction("AKTHURA_MapMetaFields", Kthura_MapMetaFields);
		Apollo_State::RequireFunction("AKTHURA_MapMeta", Kthura_MapMeta);
		Apollo_State::RequireFunction("AKTHURA_DumpBlockMap", Kthura_DumpBlockMap);
		Apollo_State::RequireFunction("AKTHURA_SpawnObjTag", Kthura_SpawnObjTag);
		Apollo_State::RequireFunction("AKTHURA_SpawnSpot", Kthura_SpawnSpot);
		Apollo_State::RequireFunction("AKTHURA_WalkTo", Kthura_WalkTo);
		Apollo_State::RequireFunction("AKTHURA_WalkToObj", Kthura_WalkToObject);
		Apollo_State::RequireFunction("AKTHURA_MoveTo", Kthura_MoveTo);
		Apollo_State::RequireFunction("AKTHURA_MoveToObj", Kthura_MoveToObject);
		Apollo_State::RequireFunction("AKTHURA_DumpDom", Kthura_DumpDom);
		Apollo_State::RequireFunction("AKTHURA_AnythingMoving", Kthura_AnyThingMoving);
		Apollo_State::RequireFunction("AKTHURA_KillObject", Kthura_KillObject);
		Apollo_State::RequireFunction("AKTHURA_GetWind", Kthura_GetWind);
		Apollo_State::RequireFunction("AKTHURA_SetWind", Kthura_SetWind);
		Apollo_State::RequireNeil("API/Kthura.neil");
	}
}