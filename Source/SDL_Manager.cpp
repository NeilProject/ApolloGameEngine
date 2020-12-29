// Lic:
// Apollo
// SDL Manager
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
// Version: 20.12.27
// EndLic


#define Apollo_SDL_QuickTest


// C or C++
#include <stdio.h>
#include <map>

// SDL
#include <SDL.h>
#include <SDL_image.h>

// JCR6
#include <jcr6_core.hpp>

// Tricky Units
#include <QuickString.hpp>

// TQSG
#include <TQSG.hpp>
#include <TQSE.hpp>
#include <TQSA.hpp>


// Apollo
#include "../Headers/SDL_Manager.hpp"
#include "../Headers/Identify.hpp"
#include "../Headers/ErrorCodes.hpp"
#include "../Headers/Crash.hpp"
#include "../Headers/States.hpp"

namespace Tricky_Apollo {

    using namespace TrickyUnits;
    using namespace jcr6;
    using namespace std;
    
    bool Apollo_SDL_Loudmouth = true;
    bool NeverFullScreen = false;

    static bool Begun_SDL = false;
    TQSG_ImageFont Apollo_SysFont;


    // static SDL_Texture* Tex_Death = NULL;
    //static std::map<std::string, TQSG_Image> Texture;
    static std::map<std::string, TQSG_AutoImage> Texture;
    static std::map<std::string, TQSA_Audio> Audio;
    static std::map<std::string, TQSG_ImageFont> Font;

    static void Apollo_SDL_Klets(std::string Gezwets) {
        if (Apollo_SDL_Loudmouth)
            printf("\x1b[31mApollo SDL>\x1b[0m %s\n", Gezwets.c_str());
    }

    static void AlgehelePaniek(std::string Paniek) {
        Crash(Paniek);
    }


    /* Likely no longer needed
    static SDL_Texture* Tex_From_JCR(JT_Dir &JD, string entry) {
        SDL_RWops* RWBuf = NULL;
        JT_Entry E = JD.Entry(entry);
        JT_EntryReader buf;
        JD.B(entry, buf);
        RWBuf = SDL_RWFromMem(buf.pointme(), buf.getsize());
        SDL_Texture* ret = IMG_LoadTexture_RW(gRenderer, RWBuf,1);
        return ret;
    }
    */

    void RemTex(std::string Tag) {
        auto T = Upper(Tag);
        //SDL_DestroyTexture(Texture[T]);        
        Texture[T]->Img()->KillAll(); // Should happen automatically on erase, but it never hurts to make 100% sure! Now that I replaced this with a shared pointer it should even go fully automatic.
        Texture.erase(T);
        Apollo_SDL_Klets("Disposed " + T);
    }

    static void RemAllTex() {
        Apollo_SDL_Klets("Removing all textures");
        for (auto &TL : Texture) {
            Apollo_SDL_Klets(TL.first);
            //SDL_DestroyTexture(TL.second);
            TL.second->Img()->KillAll(); // Should be 100% automated now due to shared pointers. This is only a safety precaution.
        }
        Apollo_SDL_Klets("All removals done");
        Texture.clear();
    }
    
    /* Shoud no longer be needed
    static void SetTex(std::string Tag, TQSG_Image &Tex) { //static void SetTex(std::string Tag, SDL_Texture* Tex) {
        auto T = Upper(Tag);
        //* Texture manager should destroy the old automatically now!
        if (Texture.count(T) == 1) {
            Apollo_SDL_Klets("Request to replace texture " + T + ", but I gotta dispose the old texture first!");
        }
        // * /
        Texture[Upper(Tag)] = Tex;        
    }
    */



    string LoadTex(std::string Tag, std::string File) {
        static unsigned int cnt=0;
        string T = Upper(Tag);
        if (Tag == "") {
            char hx[10];
            do {
                cnt++;
                sprintf_s(hx, 10,"%6X", cnt);                
                T = hx;
                T = "IMAGE::" + T;
            } while (Texture.count(T) > 0);
        }
        // SetTex(T, Tex_From_JCR(JCRPackage, File));
        /*
        TQSG_Image LTex;
        if (prefixed(Tag,"**") && suffixed(Tag,"**"))
            LTex.Create(ARF, File);
        else
        LTex.Create(JCRPackage, File);
        SetTex(T, LTex);
        */
        Texture[T] = TQSG_LoadAutoImage(JCRPackage, File);
        return T;
    }

    std::string LoadAnimTex(std::string Tag, std::string File,int w, int h, int frames) {
        auto T = LoadTex(Tag, File);
        auto Tex = GetTex(T);
        Tex->AltFrame(w, h, frames);
        return T;
    }

    void LoadTexture(std::string Tag, std::string File) {
        if (Tag == "") Crash("This version of Load Texture does require a set tag!");
        auto voider = LoadTex(Tag, File);
        // Due to a bug inside C++ using LoadTex when you don't want a new Tag returned will crash your program! At least in Visual C++ it does, as I don't know about other compilers/libraries.
    }

    void LoadAnimTexture(std::string Tag, std::string File, int w, int h, int frames) {
        if (Tag == "") Crash("This version of Load Anim Texture does require a set tag!");
        auto voider = LoadAnimTex(Tag, File, w, h, frames);
    }

    int TexHeight(std::string Tag, std::string State, std::string Traceback) {
        auto T = Upper(Tag);
        if (Texture.count(T) != 1) {
            if (T == "**DEATH**") return 0; // Otherwise we get a cyclic function call, recursing stuff forever until the stack blows up!
            Crash("TextWidth(): There is no image loaded named " + T, State, Traceback, AE_NoTextureOnTag);
            return 0;
        }
        auto& Tex = Texture[T];
        return Tex->H();
    }

    void KillTex(std::string Tag) {
        auto T = Upper(Tag);
        if (!Texture.count(Tag)) {
            cout << "\x1b[34mWARNING!\x1b[0m Request done to kill a texture tagged " << Tag << ". No texture with such a tag exists!";
        } else {
            RemTex(Tag);
        }
    }

    bool TagExists(std::string Tag) {
        return Texture.count(Upper(Tag));
    }

    std::string LoadFont(std::string Tag, std::string File, std::string State) {
        static int i = 0;
        Tag = Trim(Upper(Tag));
        if (Tag == "") {
            do {
                char NT[12];
                sprintf_s(NT, 9, "FNT%05X", i); i++;
                Tag = NT;
            } while (Font.count(Tag));
        }
        Font[Tag].LoadFont(JCRPackage, File);
        return Tag;
    }

    void LoadImageFont(std::string Tag, std::string File, std::string State) {
        if (Tag == "") Crash("No font tag when one was required", State, Apollo_State::TraceBack(State));
        auto f = LoadFont(Tag, File, State);
    }

    TQSG_ImageFont* APGetFont(std::string Tag, std::string State) {
        Tag = Upper(Trim(Tag));
        if (!Font.count(Tag)) {
            Crash("No font tagged \"" + Tag + "\"", State, Apollo_State::TraceBack(State)); return NULL;
        }
        return &Font[Tag];
    }

    bool GotFont(bool critical, std::string Tag, std::string State) {
        bool ret = Font.count(Upper(Trim(Tag)));
        if (critical && !ret) Crash("No font tagged \"" + Tag + "\"", State, Apollo_State::TraceBack(State)); 
        return ret;
    }

    void RemFont() {
        vector<string> Kill;
        for (auto tl : Font)Kill.push_back(tl.first);
        for (auto slachtoffer : Kill) RemFont(slachtoffer);
    }

    void RemFont(std::string Tag) {
        Tag = Upper(Trim(Tag));
        if (Font.count(Tag)) Font.erase(Tag);
    }

    std::string Apollo_SDL_LoadAudio(std::string Tag, std::string File, std::string State, std::string Traceback) {
        Tag = Upper(Tag);
        if (Tag == "") {
            int i = 0;
            char NT[20];
            do {
                i++;
                sprintf_s(NT, 18, "AUDIO:%08X",i);
                Tag = NT;
            } while (Audio.count(Tag));
        }
        Audio[Tag].Load(JCRPackage, File);
        if (!Audio[Tag].HasChunk()) {
            Crash("Loading Audio \"" + File + "\" failed!", State, Traceback + "\n\nJCR6 reported: \"" + Get_JCR_Error_Message() + "\"");
        }
        return Tag;
    }


    TQSA_Audio* Apollo_SDL_Audio(std::string Tag, std::string State) {
        Tag = Upper(Tag);
        if (!Audio.count(Tag)) {
            Crash("There is no audio tagged: " + Tag, State, Apollo_State::TraceBack(State));
            return NULL;
        }
        return &Audio[Tag];
    }

    void Apollo_SDL_KillAudio(std::string Tag) {
        Tag = Upper(Tag);
        if (Audio.count(Tag)) Audio.erase(Tag);
    }

    void RemAllAudio() {
        for (auto& Slachtoffer : Audio) {
            cout << "Killing audio on tag: " << Slachtoffer.first<<"\n";
            Slachtoffer.second.Kill();
        }
        Audio.clear();
    }

    TQSG_Image* GetTex(std::string Tag, std::string State) {
        if (!Texture.count(Upper(Tag))) {
            Crash("There is no image tagged: " + Tag, State, Apollo_State::TraceBack(State));
        }
        //return &Texture[Upper(Tag)];
        // Leftover from old times, but as it's too much work to get it all removed, I'll leave it this way for now. The API won't notice the difference anyway.
        return Texture[Upper(Tag)]->Img();
    }
    bool HasTex(std::string Tag) {
        return Texture.count(Upper(Tag));
    }

    int TexWidth(std::string Tag, std::string State, std::string Traceback) {
        auto T = Upper(Tag);
        if (Texture.count(T) != 1) {
            if (T == "**DEATH**") return 0; // Otherwise we get a cyclic function call, recursing stuff forever until the stack blows up!
            Crash("TextWidth(): There is no image loaded named " + T, State, Traceback, AE_NoTextureOnTag);
            return 0;
        }
        auto& Tex = Texture[T];
        return Tex->W();
    }

    void Apollo_SDL_Draw(std::string Tag, int x, int y, std::string State, std::string Traceback) {
        auto T = Upper(Tag);
        if (Texture.count(T) != 1) {
            if (T == "**DEATH**") return; // Otherwise we get a cyclic function call, recursing stuff forever until the stack blows up!
            Crash("There is no image loaded named " + T, State, Traceback, AE_NoTextureOnTag);
            return;
        }
        auto& Tex = Texture[T];
        Tex->Draw(x, y);
    }

    void Apollo_SDL_Draw(std::string Tag, int x, int y, int f, std::string State, std::string Traceback) {
        auto T = Upper(Tag);
        if (Texture.count(T) != 1) {
            if (T == "**DEATH**") return; // Otherwise we get a cyclic function call, recursing stuff forever until the stack blows up!
            Crash("There is no image loaded named " + T, State, Traceback, AE_NoTextureOnTag);
            return;
        }
        auto& Tex = Texture[T];
        Tex->Draw(x, y,f);
    }

    void Apollo_SDL_Tile(std::string Tag, int x, int y, int w, int h, std::string State, std::string Traceback) {
        Apollo_SDL_Tile(Tag, x, y, w, h, 0, State, Traceback);
    }

    void Apollo_SDL_Tile(std::string Tag, int x, int y, int w, int h, int f, std::string State, std::string Traceback) {
        auto T = Upper(Tag);
        if (Texture.count(T) != 1) {
            Crash("There is no image loaded tagged " + T, State, Traceback, AE_NoTextureOnTag);
            return;
        }
        auto& Tex = Texture[T];
        Tex->Tile(x, y, w, h, f);

    }

    void Apollo_SDL_Stretch(std::string Tag, int x, int y, int w, int h, std::string State, std::string Traceback) {
        Apollo_SDL_Stretch(Tag, x, y, w, h, 0, State, Traceback);
    }

    void Apollo_SDL_Stretch(std::string Tag, int x, int y, int w, int h, int f, std::string State, std::string Traceback) {
        auto T = Upper(Tag);
        if (Texture.count(T) != 1) {
            Crash("There is no image loaded tagged " + T, State, Traceback, AE_NoTextureOnTag);
            return;
        }
        auto& Tex = Texture[T];
        Tex->Stretch(x, y, w, h, f);
    }

    void Apollo_SDL_Flip() {
        //SDL_UpdateWindowSurface(gWindow);
        TQSG_Flip();
    }

    



    void Apollo_SDL_Start() {
        Apollo_SDL_Klets("SDL Louthmouth mode");
        printf("Starting up SDL\n\n");
        //Initialization flag
        bool success = true;

        success = TQSG_Init(Identify::WindowTitle(),Identify::WinWidth(),Identify::WinHeight(),Identify::FullScreen() && (!NeverFullScreen));
        TQSG_Panic = AlgehelePaniek; // Will direct most TQSG errors to the Apollo crash routine!
        if (!success) exit(AE_SDL_Error);
        if (!TQSA_Init(MIX_INIT_OGG)) {
            TQSG_Close();
            exit(AE_SDL_Error);
        }
        printf("SDL started succesfully\n\n");
        #ifdef Apollo_SDL_QuickTest
        // This test is will make the entire window purple, wait a few seconds and move on.
        // This test was only implemented to allow me to test stuff while the rest of the engine was still non-existent.
        //SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, 0xFF, 0x00, 0xFF));
        //TQSG_Cls();
        //Apollo_SDL_Flip();
        //SDL_Delay(5000);
        #endif
        Begun_SDL = true;
        printf("Loading SysFont\n");
        Apollo_SysFont.LoadFont(ARF, "Fonts/F25_Bank_Printer.24.jfbf");
        //if (Apollo_SysFont.HasFont()) printf("Success!"); else printf("\x1b[31mFONT ERROR>\x1b[0m %s\n", TQSG_GetError().c_str());
        printf("Initiating Event Handler\n");
        TQSE_Init();

    }

    void Apollo_SDL_End() {
        if (!Begun_SDL) {
            printf("Request to terminate SDL refused, as SDL never started in the first place\n");
            return;
        }
        printf("Preparing to terminate SDL\n");
        // Destroy all still open textures and other stuff!
        //if (Tex_Death != NULL) {
        //    printf("Removing Death from RAM\n\n");
        //    SDL_DestroyTexture(Tex_Death); Tex_Death = NULL;
        //}
        printf("Disposing SysFont\n");
        Apollo_SysFont.Kill();
        printf("Removing all loaded textures\n");
        RemFont();
        RemAllTex();
        RemAllAudio();
        // And flush the last things now
        //SDL_DestroyRenderer(gRenderer);
        //SDL_DestroyWindow(gWindow);
        //gWindow = NULL;
        printf("Terminating SDL\n");
        TQSA_Close();
        TQSG_Close();
        printf("SDL Terminated");
    }
}