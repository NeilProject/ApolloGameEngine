-- <License Block>
-- ARF_src/Neil/UseBindings.lua
-- Apollo - Use
-- version: 20.09.05
-- Copyright (C) 2020 Jeroen P. Broks
-- This software is provided 'as-is', without any express or implied
-- warranty.  In no event will the authors be held liable for any damages
-- arising from the use of this software.
-- Permission is granted to anyone to use this software for any purpose,
-- including commercial applications, and to alter it and redistribute it
-- freely, subject to the following restrictions:
-- 1. The origin of this software must not be misrepresented; you must not
-- claim that you wrote the original software. If you use this software
-- in a product, an acknowledgment in the product documentation would be
-- appreciated but is not required.
-- 2. Altered source versions must be plainly marked as such, and must not be
-- misrepresented as being the original software.
-- 3. This notice may not be removed or altered from any source distribution.
-- </License Block>



-- These are merely functions required to get things on the roll
-- Whenever the "#use" directive (or Neil.Use) is called.


local debug = false

local LString = AJCR_LoadString
local LEntries = AJCR_SpecEntries --AJCR_Entries
local LFileExists = AJCR_EntryExists
local LDirExists = AJCR_DirExists
-- As the Lua scripts come before the Neil scripts, these original
-- bindings still exist, before Neil destroys them for security reasons

function Neil.ReadFile(File)
	return LString("*MPACKAGE",Neil.Globals.ApolloState.Name,File)
end

function Neil.FileExists(File) 
	if debug then print("FileExists(\""..File.."\") ") end
	return LFileExists("*MPACKAGE",Neil.Globals.ApolloState.Name,File)
end

function Neil.DirExists(Dir)
	if debug then print("DirExists(\""..Dir.."\")") end	
	--local d = {LEntries("*MPACKAGE",Neil.Globals.ApolloState.Name)}; if debug then print("Total entries (unfiltered): ",#d) end
	--local DIR = Dir:upper()
	--if not (Neil.Globals.Suffixed(DIR,"/")) then DIR = DIR .. "/" end
	--if debug then print("Start for-loop") end
	--for _,f in ipairs(d) do
	--	if debug then print("Checking:",f,DIR,Neil.Globals.Prefixed(f:upper(),DIR)) end
	--	if Neil.Globals.Prefixed(f:upper(),DIR) then return true end
	--end
	--if debug then print("Nothing found!") end
	-- return false
	return LDirExists("*MPACKAGE",Neil.Globals.ApolloState.Name,Dir)
end

function Neil.ReadDir(Dir)
	if debug then print("ReadDir(\""..Dir.."\")") end
	--local ret = {}
	--local d = {LEntries("*MPACKAGE",Neil.Globals.ApolloState.Name)}; if debug then print("Total entries (unfiltered): ",#d) end
	--local DIR = Dir:upper()
	--if not (Neil.Globals.Suffixed(DIR,"/")) then DIR = DIR .. "/" end
	--for _,f in ipairs(d) do
	--	if Neil.Globals.Prefixed(f:upper(),DIR) then 
	--		if debug then print("Approved: ",f) end
	--		ret[#ret+1]=f 
	--	end
	--end
	ret = {LEntries("*MPACKAGE",Neil.Globals.ApolloState.Name,Dir)}
	if debug then print("Returning "..#ret.." entries") end
	return ret
end
