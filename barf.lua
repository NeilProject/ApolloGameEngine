JLS.Output("FATSTORAGE:Store") -- Might be "lzma" or "zlib" later depending on how things go here.
JLS.SetJCR6OutputFile("x64/Release/Apollo Game Engine.arf")

-- Add("ARF_src","",{Author="Jeroen P. Broks",Notes="All copyrighted by Jeroen P. Broks and zlib licensed unless explicity stated otherwise in documentation",Storage="Store"})
Add("ARF_src/ID","ID",{Author="Jeroen P. Broks",Notes="All copyrighted by Jeroen P. Broks and zlib licensed",Storage="Store"})
Add("ARF_src/Pics","Pics",{Author="Read documentation",Notes="Read Documentation",Storage="Store"})
Add("../../../lua/Neil/Neil.lua","Neil/Neil.lua",{Author="Jeroen P. Broks", Notes="zlib Licensed", Storage="Store"})

