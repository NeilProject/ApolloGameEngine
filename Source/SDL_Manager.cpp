// Lic:
// Apollo
// Init and close SDL
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

// C or C++
#include <stdio.h>

// SDL
#include <SDL.h>


// Apollo
#include "../Headers/SDL_Manager.hpp"
#include "../Headers/Identify.hpp"
#include "../Headers/ErrorCodes.hpp"

namespace Tricky_Apollo {

    //The window we'll be rendering to
    SDL_Window* gWindow = NULL;

    //The surface contained by the window
    SDL_Surface* gScreenSurface = NULL;

    void Apollo_SDL_Start() {
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
                //Get window surface
                gScreenSurface = SDL_GetWindowSurface(gWindow);
            }
        }

        if (!success) exit(AE_SDL_Error);
        printf("SDL started succesfully\n\n");
    }

    void Apollo_SDL_End() {
        printf("Preparing to terminate SDL\n");
        // TODO: Destroy all still open textures and other stuff!
        SDL_DestroyWindow(gWindow);
        gWindow = NULL;
        printf("Terminating SDL\n");
        printf("SDL Terminated");
    }
}