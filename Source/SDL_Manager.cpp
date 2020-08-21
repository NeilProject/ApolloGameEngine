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
// Version: 20.08.21
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


// Apollo
#include "../Headers/SDL_Manager.hpp"
#include "../Headers/Identify.hpp"
#include "../Headers/ErrorCodes.hpp"


namespace Tricky_Apollo {

    using namespace TrickyUnits;
    using namespace jcr6;
    
    bool Apollo_SDL_Loudmouth = true;

    static bool Begun_SDL = false;

    //The window we'll be rendering to
    static SDL_Window* gWindow = NULL;

    //The surface contained by the window
    static SDL_Surface* gScreenSurface = NULL;
    static SDL_Renderer* gRenderer = NULL;

    static SDL_Texture* Tex_Death = NULL;
    static std::map<std::string, SDL_Texture*> Texture;

    static void Apollo_SDL_Klets(std::string Gezwets) {
        if (Apollo_SDL_Loudmouth)
            printf("\x1b[31mApollo SDL>\x1b[0m %s\n", Gezwets.c_str());
    }


    static SDL_Texture* Tex_From_JCR(JT_Dir &JD, string entry) {
        SDL_RWops* RWBuf = NULL;
        JT_Entry E = JD.Entry(entry);
        JT_EntryReader buf;
        JD.B(entry, buf);
        RWBuf = SDL_RWFromMem(buf.pointme(), buf.getsize());
        SDL_Texture* ret = IMG_LoadTexture_RW(gRenderer, RWBuf,1);
        return ret;
    }

    void RemTex(std::string Tag) {
        auto T = Upper(Tag);
        SDL_DestroyTexture(Texture[T]);
        Texture.erase(T);
        Apollo_SDL_Klets("Disposed " + T);
    }

    static void RemAllTex() {
        Apollo_SDL_Klets("Removing all textures");
        for (auto TL : Texture) {
            Apollo_SDL_Klets(TL.first);
            SDL_DestroyTexture(TL.second);
        }
        Apollo_SDL_Klets("All removals done");
        Texture.clear();
    }
    
    static void SetTex(std::string Tag, SDL_Texture* Tex) {
        auto T = Upper(Tag);
        if (Texture.count(T) == 1) {
            Apollo_SDL_Klets("Request to replace texture " + T + ", but I gotta dispose the old texture first!");
        }
        
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
        SetTex(T, Tex_From_JCR(JCRPackage, File));
    }

    void Apollo_SDL_Flip() {
        SDL_UpdateWindowSurface(gWindow);
    }

    static int CLSR = 0, CLSG = 0, CLSB = 0;

    void Apollo_SDL_ClsColor(int r, int g, int b) {
        CLSR = r;
        CLSG = g;
        CLSB = b;
    }

    void Apollo_SDL_Cls() {
        SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, CLSR, CLSG, CLSB));
    }




    void Apollo_SDL_Start() {
        Apollo_SDL_Klets("SDL Louthmouth mode");
        printf("Starting up SDL\n\n");
        //Initialization flag
        bool success = true;

        //Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            //Create window
            gWindow = SDL_CreateWindow(Identify::WindowTitle().c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Identify::WinWidth() , Identify::WinHeight(), SDL_WINDOW_SHOWN);
            if (gWindow == NULL)
            {
                printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
                if (gRenderer == NULL)
                {
                    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                    success = false;
                }
                else
                {
                    //Initialize renderer color
                    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                    //Initialize PNG loading
                    int imgFlags = IMG_INIT_PNG;
                    if (!(IMG_Init(imgFlags) & imgFlags))
                    {
                        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                        success = false;
                    }
                }

                //Get window surface
                gScreenSurface = SDL_GetWindowSurface(gWindow);

                // Load Death
                printf("Loading Death Picture\n");
                Tex_Death = Tex_From_JCR(ARF, "Pics/Death.png");
            }
        }

        if (!success) exit(AE_SDL_Error);
        printf("SDL started succesfully\n\n");
        #ifdef Apollo_SDL_QuickTest
        // This test is will make the entire window purple, wait a few seconds and move on.
        // This test was only implemented to allow me to test stuff while the rest of the engine was still non-existent.
        SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, 0xFF, 0x00, 0xFF));
        Apollo_SDL_Flip();
        SDL_Delay(5000);
        #endif
        Begun_SDL = true;
    }

    void Apollo_SDL_End() {
        if (!Begun_SDL) {
            printf("Request to terminate SDL refused, as SDL never started in the first place\n");
            return;
        }
        printf("Preparing to terminate SDL\n");
        // Destroy all still open textures and other stuff!
        if (Tex_Death != NULL) {
            printf("Removing Death from RAM\n\n");
            SDL_DestroyTexture(Tex_Death); Tex_Death = NULL;
        }
        RemAllTex();
        // And flush the last things now
        SDL_DestroyWindow(gWindow);
        gWindow = NULL;
        printf("Terminating SDL\n");
        printf("SDL Terminated");
    }
}