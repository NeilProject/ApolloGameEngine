These are only a few notes and not full instructions on how to compile.
I basically recommend only to use the fully ready packages to be used, but if you want to compile all this yourself, be my guest.


Please note, I used Visual Studio to create Apollo, so the entire project has been set up for that. I do not know how well this engine will perform on other systems. Since I cannot test that out, I will not yet opt for that.



A few things are required in order to compile:

- 3rd party packages
  - SDL2
  - SDL2_image
  - SDL2_mixer
  - Lua
- My own libraries
  - [Tricky's Units for C++](https://github.com/Tricky1975/TrickyUnits_CPP)
  - [TQSG](https://github.com/TrickyGameTools/TQSG)
  - [JCR6 for C++](https://github.com/jcr6/JCR6_CPP)
    - Note: The zlib functionality is for now set on-hold, as Visual Studio refuses to compile zlib on me.
  - [Kthura for C++](https://github.com/TrickyGameTools/Kthura-for-CPP)
  
Please note that my Visual Studio solution and project and filter files are set to my personal directory settings which may not necessarily match your own.



Apollo requires it's ARF file (which must always have the same name as the executable but with the .exe extension replaced by .arf - Unix/Linux uses should note that if you use . in the name of the executable name the system can get mistaken.
The file must also be in the same directory as the executable file, yes also on Mac and Linux. The ARF file is not included in this repository. With use use of [NJCR](https://github.com/jcr6/NJCR) and barf.lua (alternatively that file should also work with the JCR6 tools written in go), the arf file can get created. Please note that [Neil](https://github.com/NeilProject/Neil) must also be present (it only needs Neil.lua) in order to build the ARF file.

