
#define UNITY_BUILD 1
#include <stdio.h>              // IWYU pragma: keep
#include <string.h>             // IWYU pragma: keep
#include <assert.h>             // IWYU pragma: keep
#include <stdbool.h>            // IWYU pragma: keep
#include <inttypes.h>
#ifdef _WIN64
 #include <SDL.h>
 #include <SDL_image.h>
#else
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_image.h>
#endif
#include "p2d_globals.h"
#include "p2d_common.h"
#include "p2d_memory.h"
#include "p2d_sdl_utils.h"
#include "p2d_characters.h"
#include "p2d_camera.h"
#include "p2d_tilemap.h"
#include "p2d_gamestate.h"
#include "p2d_render.h"


int
main(int argc, char *args[])
{
    Arena arena = MakeArena();

    SDL_Window *window;
    SDL_Renderer *renderer;
    // Init SDL without texture filtering for better pixelart results
    if (sdl_utils_Init("SDL Tutorial", &window, &renderer, 0)) {
        Gamestate *gamestate = resources_LoadResources_MakeGamestate(&arena, renderer);

        Uint64 lastTicks = 0;
        while (1) {            
            // Update SDL events
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    SDL_DestroyTexture(gamestate->player.animations->spritesTexture);
                    SDL_DestroyTexture(gamestate->tilemap.resources->tilesTexture);
                    sdl_utils_Quit(window, renderer);
                    return 0;
                }

                character_ProcessKeyboardEvents(&gamestate->player, &event);
            }

            // Fixed FPS
            while (1) {
                Uint64 ticks = SDL_GetTicks64();
                Uint64 current = ticks - lastTicks;
                if (current == TICKS_FPS) {
                    break;
                } else if (current > TICKS_FPS) {
                    printf("Slow by %" PRId64 " ms\n", current - TICKS_FPS);
                    break;
                } else {
                    Uint64 missingDelay = TICKS_FPS - current - 1;
                    if (missingDelay > 1) {
                        SDL_Delay(missingDelay);
                    }
                }
            }
            lastTicks = SDL_GetTicks64();

            // Update
            character_Update(&gamestate->player);
            camera_Update(&gamestate->camera);

            render_Draw(gamestate);            
        }
    }
    
    return 0;
}
