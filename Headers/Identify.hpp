// Lic:
// Apollo
// Identify
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
// Version: 20.08.26
// EndLic
// Tricky's Units
#ifndef APOLLO_INCLUDED_ID
#define APOLLO_INCLUDED_ID
#include <GINIE.hpp>
#include <Globals.hpp>
namespace Tricky_Apollo {

	enum FlowType {
		UnknownFlow,
		RegularRepeativeFlow,
		SemiCallback,
		FullCallback
	};

	class Identify {
	private:
		static TrickyUnits::GINIE ConfigData;
	public:
		static void LoadIdentify();
		static std::string MetaData(std::string key);
		static std::string EngineData(std::string key);
		static std::string WindowTitle();
		static std::string ProjectData(std::string Key);
		static bool FullScreen();		
		static int WinHeight();
		static int WinWidth();
		static FlowType GetFlowType();
	};

}
#endif