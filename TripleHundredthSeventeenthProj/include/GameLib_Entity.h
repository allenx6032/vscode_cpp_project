// Copyright (C) 2011-2023 Valeriano Alfonso Rodriguez (Kableado)

#ifndef Entity_H
#define Entity_H

#include "GameLib_Anim.h"
#include "GameLib_Bucket.h"
#include "GameLib_Draw.h"
#include "GameLib_Util.h"
#include "GameLib_Component.h"

////////////////////////////////////////////////
// Entity
//
#define EntityFlag_Collision 1
#define EntityFlag_Overlap 2
#define EntityFlag_Light 4
#define EntityFlag_BlockTop 0x00000100
#define EntityFlag_BlockRight 0x00000200
#define EntityFlag_BlockBottom 0x00000400
#define EntityFlag_BlockLeft 0x00000800
#define EntityFlag_Block 0x0000FF00
#define EntityFlag_PlatformCollision 0x00000101
#define EntityFlag_BlockCollision 0x0000FF01

// TODO: Need to confirm these values and add others if they exist
// For now, assuming EntityIntFlag_UpdatedScale is 16
// and EntityIntFlag_UpdatedPosition is handled differently or defined elsewhere
// #define EntityIntFlag_UpdatedPosition 1 // Example, if needed
// #define EntityIntFlag_UpdatedScale 16 // Example, if needed
#define EntityIntFlag_UpdatedRotation 32

typedef struct TEntity TEntity, *Entity;
struct TEntity {
	Entity base;

	int type;
	int flags;
	int internalFlags;
	int zorder;
	float sortYOffset;

	EntBody *body;
	EntSprite *sprite;

	void (*oncopy)(Entity ent);
	void (*oninit)(Entity ent);
	void (*ondelete)(Entity ent);
	void (*proc)(Entity ent, int ft);
	void (*postproc)(Entity ent, int ft);
	int (*collision)(Entity ent, Entity ent2, float t, vec2 n);
	void (*overlap)(Entity ent, Entity ent2);

	int A;
	int B;
	int C;
	int D;
	int E;
	vec2 VecA;
	vec2 VecB;
	Entity child;

	TBBox bbox;

	Entity next;
};

/////////////////////////////
// Entity_GetFree
//
Entity Entity_GetFree();

/////////////////////////////
// Entity_New
//
Entity Entity_New();

/////////////////////////////
// Entity_Init
//
Entity Entity_Init(Entity e);

/////////////////////////////
// Entity_Destroy
//
void Entity_Destroy(Entity e);

/////////////////////////////
// Entity_Copy
//
Entity Entity_Copy(Entity e);

/////////////////////////////
// Entity_CalcBBox
//
//
void Entity_CalcBBox(Entity e);

/////////////////////////////
// Entity_BBoxIntersect
//
//
int Entity_BBoxIntersect(Entity ent1, Entity ent2);

/////////////////////////////
// Entity_Draw
//
void Entity_Draw(Entity e, int x, int y, float f);

/////////////////////////////
// Entity_IsVisible
//
int Entity_IsVisible(Entity e, int x, int y, int w, int h);

/////////////////////////////
// Entity_Process
//
void Entity_Process(Entity e, int ft);

/////////////////////////////
// Entity_PostProcess
//
void Entity_PostProcess(Entity e, int ft);

////////////////////////////////////////////////
// CollisionInfo
//
#define CollisionResponse_Circle 1
#define CollisionResponse_Line 2
typedef struct TCollisionInfo TCollisionInfo, *CollisionInfo;
struct TCollisionInfo {
	int responseType;
	Entity ent1;
	Entity ent2;
	float t;
	vec2 n;
	int applyFriction;

	CollisionInfo next;
};

/////////////////////////////
// CollisionInfo_New
//
//
CollisionInfo CollisionInfo_New(int responseType, Entity ent1, Entity ent2, float t, const vec2 n, int applyFriction);

/////////////////////////////
// CollisionInfo_Destroy
//
//
void CollisionInfo_Destroy(CollisionInfo *collInfoRef);

/////////////////////////////
// CollisionInfo_Add
//
//
void CollisionInfo_Add(
	CollisionInfo *collInfo, int responseType, Entity ent1, Entity ent2, float t, vec2 n, int applyFriction);

/////////////////////////////
// CollisionInfo_CheckRepetition
//
//
int CollisionInfo_CheckRepetition(CollisionInfo collInfo, Entity ent1, Entity ent2);

/////////////////////////////
// Entity_CheckCollision
//
//
int Entity_CheckCollision(Entity ent1, Entity ent2, CollisionInfo *collInfoRef);

/////////////////////////////
// Entity_CollisionResponseClircle
//
// Normal response to a collision of spheres.
void Entity_CollisionResponseCircle(Entity b1, Entity b2, float t, const vec2 n);

/////////////////////////////
// Entity_CollisionResponseLine
//
// Normal response to a collision with a line.
void Entity_CollisionResponseLine(Entity ent, Entity ent2, float t, vec2 n, int applyFriction);

/////////////////////////////
// Entity_CollisionInfoResponse
//
//
int Entity_CollisionInfoResponse(CollisionInfo collInfo);

/////////////////////////////
// Entity_Overlaps
//
void Entity_Overlaps(Entity b1, Entity b2);

/////////////////////////////
// Entity_GetPos
// Entity_SetPos
// Entity_AddPos
// Entity_UpdatePos
//
void Entity_GetPos(Entity e, vec2 pos);
void Entity_SetPos(Entity e, const vec2 pos);
void Entity_AddPos(Entity e, const vec2 pos);
void Entity_UpdatePos(Entity e, const vec2 pos);

/////////////////////////////
// Entity_AddVel
// Entity_SetVel
// Entity_SetVelH
// Entity_SetVelV
// Entity_AddVelLimit
// Entity_AddVelLimitH
// Entity_AddVelLimitH
//
void Entity_AddVel(Entity e, const vec2 vel);
void Entity_SetVel(Entity e, const vec2 vel);
void Entity_SetVelH(Entity e, float v);
void Entity_SetVelV(Entity e, float v);
void Entity_AddVelLimit(Entity e, const vec2 vel, float limit);
void Entity_AddVelLimitH(Entity e, float v, float limit);
void Entity_AddVelLimitV(Entity e, float v, float limit);

/////////////////////////////
// Entity_SetColor
// Entity_AddColor
// Entity_MultColor
// Entity_AddColor
// Entity_SetDefaultColor
//
void Entity_SetColor(Entity e, float r, float g, float b, float a);
void Entity_AddColor(Entity e, float r, float g, float b, float a);
void Entity_MultColor(Entity e, float r, float g, float b, float a);
void Entity_SetLight(Entity e, float r, float g, float b, float rad);
void Entity_SetDefaultColor(Entity e, float r, float g, float b, float a);

/////////////////////////////
// Entity_SetScale
// Entity_GetScale
//
void Entity_SetScale(Entity e, const float scale[2]);
void Entity_GetScale(Entity e, float scale[2]);

/////////////////////////////
// Entity_SetRotation
// Entity_GetRotation
// Entity_AddRotation
//
void Entity_SetRotation(Entity e, float angle);
void Entity_GetRotation(Entity e, float *angle);
void Entity_AddRotation(Entity e, float angle);

/////////////////////////////
// Entity_Iluminate
//
void Entity_Iluminate(Entity e, Entity *entityList, int n);

/////////////////////////////
// Entity_MarkUpdateLight
//
void Entity_MarkUpdateLight(Entity e, Entity *elist, int n);

/////////////////////////////
// Entity_IsLight
//
int Entity_IsLight(Entity e);

/////////////////////////////
// Entity_IsUpdateLight
//
int Entity_IsUpdateLight(Entity e);

/////////////////////////////
// Entity_IsMoving
//
int Entity_IsMoving(Entity e);

#endif
