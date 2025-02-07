#ifndef UNITY_BUILD
 #pragma once
 #ifdef _WIN64
  #include <SDL.h>
  #include <SDL_image.h>
 #else
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_image.h> 
 #endif
 #include "p2d_globals.h"
 #include "p2d_camera.h"
#endif

//
// Tile map
//

#define MAX_MAP_SIZE 32

struct {
    int tileSize;    
    SDL_Texture *tilesTexture;

    SDL_Rect *tilesRects;
    int tileRectsCount;
} typedef TilemapResources;

struct {    
    int tiles[MAX_MAP_SIZE][MAX_MAP_SIZE];
    TilemapResources *resources;    
} typedef Tilemap;


//
// Tilemap resources
//
inline function TilemapResources*
resources_LoadTilemapResources(Arena *arena, SDL_Renderer *renderer)
{
    TilemapResources *resources = (TilemapResources *) ReserveMemory(arena, sizeof(TilemapResources));
    resources->tilesTexture = IMG_LoadTexture(renderer, "res/sheet.png");
    resources->tileSize = 16;

    resources->tilesRects = (SDL_Rect *) ReserveMemory(arena, sizeof(SDL_Rect));
    *resources->tilesRects = (SDL_Rect) {129, 0, 16, 16};
    resources->tileRectsCount = 1;

    return resources;
}

inline function Tilemap
resources_LoadTilemap(TilemapResources *resources)
{
    Tilemap tilemap = {};
    tilemap.resources = resources;

    // Load file
    size_t dataSize;
    void *mapContents = SDL_LoadFile("res/map.p2d", &dataSize);

    // Parse it
    int mapX = 0;
    int mapY = 0;
    char *current = (char *) mapContents;
    char *currentNumber = current;
    while (*current != '\0') {
        const bool newLine = *current == '\n';

        if (*current == ',' || newLine) {
            *current = '\0';
            tilemap.tiles[mapY][mapX] = strtol(currentNumber, 0, 10);
            currentNumber = current + 1;
            ++mapX;
        }

        if (newLine) {
            ++mapY;
            mapX = 0;
        }
        ++current;
    }
    
    SDL_free(mapContents);
    return tilemap;
}
