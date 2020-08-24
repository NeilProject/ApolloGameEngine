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
// Version: 20.08.25
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


// Apollo
#include "../Headers/SDL_Manager.hpp"
#include "../Headers/Identify.hpp"
#include "../Headers/ErrorCodes.hpp"
#include "../Headers/Crash.hpp"

namespace Tricky_Apollo {

    using namespace TrickyUnits;
    using namespace jcr6;
    
    bool Apollo_SDL_Loudmouth = true;

    static bool Begun_SDL = false;
    TQSG_ImageFont Apollo_SysFont;


    // static SDL_Texture* Tex_Death = NULL;
    static std::map<std::string, TQSG_Image> Texture;

    static void Apollo_SDL_Klets(std::string Gezwets) {
        if (Apollo_SDL_Loudmouth)
            printf("\x1b[31mApollo SDL>\x1b[0m %s\n", Gezwets.c_str());
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
        Texture[T].KillAll(); // Should happen automatically on erase, but it never hurts to make 100% sure!
        Texture.erase(T);
        Apollo_SDL_Klets("Disposed " + T);
    }

    static void RemAllTex() {
        Apollo_SDL_Klets("Removing all textures");
        for (auto &TL : Texture) {
            Apollo_SDL_Klets(TL.first);
            //SDL_DestroyTexture(TL.second);
            TL.second.KillAll();
        }
        Apollo_SDL_Klets("All removals done");
        Texture.clear();
    }
    
    static void SetTex(std::string Tag, TQSG_Image &Tex) { //static void SetTex(std::string Tag, SDL_Texture* Tex) {
        auto T = Upper(Tag);
        /* Texture manager should destroy the old automatically now!
        if (Texture.count(T) == 1) {
            Apollo_SDL_Klets("Request to replace texture " + T + ", but I gotta dispose the old texture first!");
        }
        */
        Texture[Upper(Tag)] = Tex;
        
    }

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
        TQSG_Image LTex;
        if (prefixed(Tag,"**") && suffixed(Tag,"**"))
            LTex.Create(ARF, File);
        else
        LTex.Create(JCRPackage, File);
        SetTex(T, LTex);
    }

    void Apollo_SDL_Draw(std::string Tag, int x, int y, std::string State, std::string Traceback) {
        auto T = Upper(Tag);
        if (Texture.count(T) != 1) {
            if (T == "**DEATH**") return; // Otherwise we get a cyclic function call, recursing stuff forever until the stack blows up!
            Crash("There is no image loaded named " + T, State, Traceback, AE_NoTextureOnTag);
            return;
        }
        auto& Tex = Texture[T];
        Tex.Draw(x, y);
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

        success = TQSG_Init(Identify::WindowTitle(),Identify::WinWidth(),Identify::WinHeight());
        if (!success) exit(AE_SDL_Error);
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
        printf("Loading SysFont -- ");
        Apollo_SysFont.LoadFont(ARF, "Fonts/F25_Bank_Printer.24.jfbf");
        //if (Apollo_SysFont.HasFont()) printf("Success!"); else printf("\x1b[31mFONT ERROR>\x1b[0m %s\n", TQSG_GetError().c_str());

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
        RemAllTex();
        // And flush the last things now
        //SDL_DestroyRenderer(gRenderer);
        //SDL_DestroyWindow(gWindow);
        //gWindow = NULL;
        printf("Terminating SDL\n");
        TQSG_Close();
        printf("SDL Terminated");
    }
}