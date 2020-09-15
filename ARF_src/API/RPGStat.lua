-- <License Block>
-- ARF_src/API/RPGStat.lua
-- RPG Stat Link up
-- version: 20.09.15
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
-- RPG Char LUA
local CharsWantedBefore={} -- Saves time later!
local PreciseWanted={}
local RPGCreate = RPGCreate _G.RPGCreate=nil
local sf = string.format

Neil.Load([[
QuickMeta RPGParty

	index
		return Lua.RPGGetParty(key)
	end
	
	newindex
		if type(key)=="string" and key:upper()=="MAX"
			cout("Setting Party.Max to ",value,"\t",Lua.type(value),"\n")
			 Lua.RPGSetPartyMax(value)
			 return
		end
		Lua.RPGSetParty(key,value)
	end
end]])()

RPGChar = setmetatable({},{

	__newindex = function() error("No new members may be assinged to RPGChar") end,

	__index = function(s,char)
		if char:upper()=="CREATE" then return RPGCreate end
		CharsWantedBefore[char] = CharsWantedBefore[char] or setmetatable({},{
			__newindex = function() error("No new data may be appointed to characters in this manner") end,
			__index = function(s,what)
					what = what:upper()
					if what=="STAT" then 
						PreciseWanted[sf("%s.STAT",char)] = PreciseWanted[sf("%s.STAT",char)] or setmetatable({},{
							__index = function(s,stat) return RPGGetStatValue(char,stat) end,
							__newindex = function(s,stat,value) RPGSetStatValue(char,stat,value) end
						})
						return PreciseWanted[sf("%s.STAT",char)] 
					elseif what=="STATSCRIPT" then
						PreciseWanted[sf("%s.STATSCRIPT",char)] = PreciseWanted[sf("%s.STATSCRIPT",char)] or setmetatable({},{
							__index = function(s,stat) error("Reading a write-only value") end,
							__newindex = function(s,stat,value) RPGSetStatScript(char,stat,value) end
						})
						return PreciseWanted[sf("%s.STATSCRIPT",char)]
					elseif what=="POINTS" then
						PreciseWanted[sf("%s.POINTS",char)] = PreciseWanted[sf("%s.POINTS",char)] or setmetatable({},{
							__newindex = function() error("Read only field assigned") end,
							__index = function(s,points)
								PreciseWanted[sf("%s.POINTS.%s",char,points)] = PreciseWanted[sf("%s.POINTS.%s",char,points)] or setmetatable({},{
									__newindex = function(s,field,value)
										RPGSetPoints(char,points,field,value)
									end,
									__index = function(s,field)
										return RPGGetPoints(char,points,field)
									end
								})
								return PreciseWanted[sf("%s.POINTS.%s",char,points)] 
							end

						})
						return PreciseWanted[sf("%s.POINTS",char)]
					elseif what=="DATA" then
						PreciseWanted[sf("%s.DATA",char)] = PreciseWanted[sf("%s.DATA",char)] or setmetatable({},{
							__index = function(s,data) return RPGGetData(char,data) end,
							__newindex = function(s,data,value) RPGSetData(char,data,value) end
						})
						return PreciseWanted[sf("%s.DATA",char)] 
					elseif what=="LIST" then
						PreciseWanted[sf("%s.LIST",char)] = PreciseWanted[sf("%s.LIST",char)] or setmetatable({},{
							__newindex = function(s,list,value)
								if value==nil then
									RPGKillList(char,list)
								else
									error("That value cannot be used for list manipulation")
								end
							end,
							__index = function(s,list)
								PreciseWanted[sf("%s.LIST.%s",char,list)] = PreciseWanted[sf("%s.LIST.%s",char,list)] or setmetatable({},{
									__len = function(s)
										return RPGListCount(char,list)
										-- This only works on Lua 5.2 or later, and this game uses Lua 5.4 so this should be alright
									end,
									__index = function(s,index)
										if type(index)=="number" then
											return RPGListGet(char,list,index)
										elseif index:upper()=="EACH" then
											local iindex=-1
											local size=RPGListCount(char,list)
											return function()
												iindex = iindex + 1
												if iindex>=size then return nil end
												return RPGListGet(char,list,iindex)
											end										
										else
											error("Unknown manipulation command!")
										end
									end,
									__newindex = function(s,index,value)
										if type(index)=="number" then
											RPGListSet(char,list,index,value)
										elseif index:upper()=="ADD" or index:upper()=="NEW" then
											RPGListAdd(char,list,value)
										else
											error("Unknown manipulation command!")
										end
									end
								})
								return PreciseWanted[sf("%s.LIST.%s",char,list)] 
							end
						})
						return PreciseWanted[sf("%s.LIST",char)]
					end
				end		
			})
			return CharsWantedBefore[char] 
		end
	})

Neil.Load("global const table RPGChar = Lua.RPGChar")()