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
 #include "p2d_characters.h"
 #include "p2d_camera.h"
 #include "p2d_tilemap.h"
#endif


//
// Game state
//
struct {
    SDL_Renderer *renderer;
    Character player;
    Tilemap tilemap;
    Camera camera;
} typedef Gamestate;


inline function Gamestate *
resources_LoadResources_MakeGamestate(Arena *arena, SDL_Renderer *renderer)
{
    Gamestate *gamestate = (Gamestate*) ReserveMemory(arena, sizeof(Gamestate));
    gamestate->camera = camera_Make(&gamestate->player);
    gamestate->renderer = renderer;

    // Player character
    CharacterAnimations *animations = resources_LoadCharacterAnimations(arena, renderer);
    gamestate->player = character_MakePlayer(animations);

    // Tile map
    TilemapResources *tilemapResources = resources_LoadTilemapResources(arena, renderer);
    gamestate->tilemap = resources_LoadTilemap(tilemapResources);    

    return gamestate;
}
