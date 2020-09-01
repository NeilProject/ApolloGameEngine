-- <License Block>
-- barf.lua
-- Build Apollo Resource File
-- version: 20.08.31
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
local PackAs = "Store"
local dir = "E:/Projects/Applications/VisualStudio/VC/Apollo Game Engine/" -- Of course this may need to be adapted based on your own system settings

JLS.Output("FATSTORAGE:Store") -- Might be "lzma" or "zlib" later depending on how things go here.
JLS.SetJCR6OutputFile(dir.."x64/Release/Apollo Game Engine.arf")

-- Add("ARF_src","",{Author="Jeroen P. Broks",Notes="All copyrighted by Jeroen P. Broks and zlib licensed unless explicity stated otherwise in documentation",Storage=PackAs})
Add(dir.."ARF_src/ID","ID",{Author="Jeroen P. Broks",Notes="All copyrighted by Jeroen P. Broks and zlib licensed",Storage=PackAs})
Add(dir.."ARF_src/Pics","Pics",{Author="Read documentation",Notes="Read Documentation",Storage=PackAs})
Add(dir.."ARF_src/Fonts","Fonts",{Author="Volker Busse",Notes="Donated to DaFont as 100% free",Storage=PackAs})
Add(dir.."../../../lua/Neil/Neil.lua","Neil/Neil.lua",{Author="Jeroen P. Broks", Notes="zlib Licensed", Storage=PackAs})
Add(dir.."ARF_src/Neil","Neil",{Author="Jeroen P. Broks",Notes="zlib Licensed",Storage=PackAs})
Add(dir.."ARF_src/API","API",{Author="Jeroen P. Broks",Notes="zlib Licensed",Storage=PackAs})