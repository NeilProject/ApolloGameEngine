// Lic:
// Apollo
// Error codes
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

#ifndef APOLLO_INCLUDED_ERRORS
#define APOLLO_INCLUDED_ERRORS


namespace Tricky_Apollo {
	enum {
		AE_NoError = 0,
		AE_CLI_Arg_Error,
		AE_NoPackage,
		AE_JCR6Error,
		AE_NoIdentify,
		AE_IdentifyError,
		AE_VersionTooLow,
		AE_ARF_NotFound,
		AE_ARF_Invalid,
		AE_SDL_Error,
		AE_Script_Error,
		AE_NoMainScript,
		AE_NoTextureOnTag,
		AE_NoAudioOnTag,
		AE_LuaStateCreationFailure,
		AE_General_Error
	};
}
#endif