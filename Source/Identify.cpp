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
// Version: 20.08.21
// EndLic
#include <QuickString.hpp>
#include "..\Headers\Identify.hpp"


namespace Tricky_Apollo {
	using namespace std;
	using namespace TrickyUnits;

	TrickyUnits::GINIE Identify::ConfigData;

	void Identify::LoadIdentify() {
		auto ID_Script = JCRPackage.Lines("ID/Identify.ini");
		ConfigData.Parse(ID_Script);
	}
	string Identify::MetaData(std::string key)	{
		return ConfigData.Value("META", key);
	}
	string Identify::EngineData(std::string key)	{
		return Upper(ConfigData.Value("ENGINE",key));
	}
	
}