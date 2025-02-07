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
 #include "p2d_characters.h"
#endif

//
// Camera
//

struct {
    int x, y;
    int w, h;
    Character *focus;
} typedef Camera;


inline function Camera
camera_Make(Character *focus) 
{
    Camera camera = {};
    camera.focus = focus;
    return camera;
}

inline function void
camera_Update(Camera *camera)
{
    camera->x = camera->focus->x - SCREEN_W_CAM + camera->focus->w / 2;
    camera->y = camera->focus->y - SCREEN_H_CAM;
}
