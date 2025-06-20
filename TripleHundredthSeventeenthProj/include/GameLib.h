// Copyright (C) 2011-2023 Valeriano Alfonso Rodriguez (Kableado)

#ifndef GameLib_H
#define GameLib_H

#include "GameLib_Anim.h"
#include "GameLib_Audio.h"
#include "GameLib_Draw.h"
#include "GameLib_Entity.h"
#include "GameLib_Input.h"
#include "GameLib_TimeUtils.h"
#include "GameLib_Util.h"

/////////////////////////////
// GameLib_Init
//
// Initializes the game.
int GameLib_Init(int w, int h, char *title, int pFps, int fps);

/////////////////////////////
// GameLib_PrepareSize
//
//
void GameLib_PrepareSize(float x1, float y1, float x2, float y2);

/////////////////////////////
// GameLib_CheckSize
//
//
void GameLib_CheckSize();

/////////////////////////////
// GameLib_AddEntity
//
// Adds an entity to the game.
void GameLib_AddEntity(Entity e);

/////////////////////////////
// GameLib_UnrefEntity
//
// removes the reference to the entity.
int GameLib_UnrefEntity(Entity e);

/////////////////////////////
// GameLib_DelEntity
//
// Adds an entity to the game.
int GameLib_DelEntity(Entity e);

/////////////////////////////
// GameLib_Loop
//
// Loops the game.
void GameLib_Loop(void (*gameProc)(), void (*gamePostProc)(), void (*gamePreDraw)(float f), void (*gameDraw)(float f));

/////////////////////////////
// GameLib_GetPos
// GameLib_SetPos
// GameLib_UpdatePos
// GameLib_SetPos
// GameLib_GetPosInstant
// GameLib_SetPosOffset
//
//
void GameLib_GetPos(int pos[2]);
void GameLib_SetPos(const int pos[2]);
void GameLib_UpdatePos(const int pos[2]);
void GameLib_GetSize(int size[2]);
void GameLib_GetPosInstant(int pos[2], float f);
void GameLib_SetPosOffset(const int posOffset[2]);

/////////////////////////////
// GameLib_MoveToPos
// GameLib_MoveToPosH
// GameLib_MoveToPosV
//
//
void GameLib_MoveToPos(vec2 pos, float f);
void GameLib_MoveToPosH(const vec2 pos, float f);
void GameLib_MoveToPosV(const vec2 pos, float f);

/////////////////////////////
// GameLib_ForEachEn
//
// Deletes every entity.
void GameLib_DelEnts();

/////////////////////////////
// GameLib_ForEachEnt
//
// Iterates every entity.
void GameLib_ForEachEnt(int (*func)(Entity ent));

/////////////////////////////
// GameLib_SearchEnt
//
// Searches throught the entities.
Entity GameLib_SearchEnt(int (*func)(Entity ent, void *d), void *d);

/////////////////////////////
// GameLib_EntityCustomCheckCollision
//
//
int GameLib_EntityCustomCheckCollision(Entity ent, vec2 vel);

/////////////////////////////
// GameLib_PlaySound
//
// Play a sound position aware.
void GameLib_PlaySound(AudioSnd snd, int x, int y);

/////////////////////////////
// GameLib_PlayLoopingSound
//
// Play a sound looping
AudioChn GameLib_PlayLoopingSound(AudioSnd snd);

/////////////////////////////
// GameLib_EntitySetLight
//
//
void GameLib_EntitySetLight(Entity e, float r, float g, float b, float rad);

/////////////////////////////
// GameLib_ConvertScreenPositionToGamePosition
//
//
void GameLib_ConvertScreenPositionToGamePosition(vec2 screenPos, vec2 gamePos, float f);

/////////////////////////////
// GameLib_AddParallaxBackground
//
//
void GameLib_AddParallaxBackground(DrawImg img, int imgSize[2], int imgOffset[2], float parallaxFactor[2]);

/////////////////////////////
// GameLib_CleanParallaxBackgrounds
//
//
void GameLib_CleanParallaxBackgrounds();

#endif
