#ifndef UNITY_BUILD
 #pragma once
 #ifdef _WIN64
  #include <SDL.h>
  #include <SDL_image.h>
 #else
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_image.h>
 #endif
 #include "p2d_memory.h"
 #include "p2d_globals.h"
 #include "p2d_gamestate.h"
#endif

inline function void
render_Draw(Gamestate *gamestate)
{
    SDL_Renderer *renderer = gamestate->renderer;
    Tilemap *tilemap = &gamestate->tilemap;
    Character *player = &gamestate->player;
    Camera *camera = &gamestate->camera;
    
    SDL_RenderClear(renderer);

    //
    // Tilemap
    //
    int tileSize = tilemap->resources->tileSize;
    int targetSize = tileSize * RENDER_SCALE;
    for (int i = 0; i < MAX_MAP_SIZE; ++i) {
        for (int j = 0; j < MAX_MAP_SIZE; ++j) {
            int CurrentTile = tilemap->tiles[j][i] - 1; // As 0 means empty, adjust CurrentTile value 
            if (CurrentTile == -1) 
                continue;

            SDL_Rect *srcRect = tilemap->resources->tilesRects;
            for (int r = 0; r < tilemap->resources->tileRectsCount && r < CurrentTile - 1; ++r) {
                ++srcRect;
            }

            SDL_Rect destRect;
            destRect = (SDL_Rect){ i * targetSize - camera->x, j * targetSize - camera->y, targetSize, targetSize};            
            SDL_RenderCopy(renderer, tilemap->resources->tilesTexture, srcRect, &destRect);
        }
    }

    //
    // Character
    //
    SDL_Rect *currentSrcRect = character_GetCurrentSprite(player);
    SDL_Rect destRect;
    destRect.x = player->x - camera->x;
    destRect.y = player->y - camera->y;
    destRect.w = currentSrcRect->w * RENDER_SCALE;
    destRect.h = currentSrcRect->h * RENDER_SCALE;
    SDL_RenderCopy(renderer, player->animations->spritesTexture, currentSrcRect, &destRect);
    
    SDL_RenderPresent(renderer);
}
