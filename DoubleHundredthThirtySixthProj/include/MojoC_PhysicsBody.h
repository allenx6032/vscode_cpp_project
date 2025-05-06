/*
 * Copyright (c) scott.cgi All Rights Reserved.
 *
 * This source code belongs to project Mojoc, which is a pure C Game Engine hosted on GitHub.
 * The Mojoc Game Engine is licensed under the MIT License, and will continue to be iterated with coding passion.
 *
 * License  : https://github.com/scottcgi/Mojoc/blob/master/LICENSE
 * GitHub   : https://github.com/scottcgi/Mojoc
 * CodeStyle: https://github.com/scottcgi/Mojoc/blob/master/Docs/CodeStyle.md
 *
 * Since    : 2014-6-3
 * Update   : 2019-1-18
 * Author   : scott.cgi
 */


#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H


#include "MojoC_Matrix.h"
#include "MojoC_Bitwise.h"
#include "MojoC_Array.h"
#include "MojoC_UserData.h"


typedef enum
{
    PhysicsShape_NULL    = 0,
    PhysicsShape_Polygon = 1,
    PhysicsShape_Line    = 1 << 2,
    PhysicsShape_Point   = 1 << 3,
}
PhysicsShape;


typedef enum
{
    /**
     * Not add in physics world yet.
     */
    PhysicsBodyState_OutsideWorld,

    /**
     * Can motion can collision.
     */
    PhysicsBodyState_Normal,

    /**
     * No motion can collision.
     */
    PhysicsBodyState_Fixed,

    /**
     * No motion no collision.
     */
    PhysicsBodyState_Freeze,
}
PhysicsBodyState;


typedef struct PhysicsBody PhysicsBody;
struct  PhysicsBody
{
    UserData         userData[1];

    /**
     * Used to identify PhysicsBody, default -1.
     */
    int              userID;

    float            positionX;
    float            positionY;
    float            velocityX;
    float            velocityY;
    float            accelerationX;
    float            accelerationY;
    float            rotationZ;

    PhysicsShape     shape;

    /**
     * PhysicsBody current state.
     */
    PhysicsBodyState state;

    /**
     * Pow of 2, default 0.
     * body can collision between different collisionGroup (no same bit).
     */
    int              collisionGroup;

    /**
     * Store born vertices.
     */
    Array(float)     vertexArr[1];

    /**
     * The vertices after transformed.
     */
    Array(float)     transformedVertexArr[1];

    /**
     * When body collision callback.
     */
    void (*OnCollision)(PhysicsBody* self, PhysicsBody* other, float deltaSeconds);
};


/**
 * Control PhysicsBody.
 */
struct APhysicsBody
{
    /**
     * Create body with shape and vertices.
     *
     * the vertexArr will copy into body,
     * and the body's vertexArr and transformedVertexArr are same when init,
     * and all data create by one malloc.
     *
     * if shape not support will return NULL.
     */
    PhysicsBody* (*Create)       (PhysicsShape shape, Array(float)* vertexArr);

    /**
     * Reset body's transformedVertexArr to vertexArr.
     */
    void         (*ResetVertices)(PhysicsBody* body);

    /**
     * Simulate body motion and update transformedVertexArr by position, rotation.
     */
    void         (*Update)       (PhysicsBody* body, float deltaSeconds);

};


extern struct APhysicsBody APhysicsBody[1];


//----------------------------------------------------------------------------------------------------------------------


/**
 *  Check physicsBody whether has same bit in collisionGroup.
 */
static inline bool APhysicsBody_CheckCollisionGroup(PhysicsBody* physicsBody, int collisionGroup)
{
    return ABitwise_Check(physicsBody->collisionGroup, collisionGroup);
}


/**
 * Add collisionGroup to physicsBody.
 */
static inline void APhysicsBody_AddCollisionGroup(PhysicsBody* physicsBody, int collisionGroup)
{
    ABitwise_Add(physicsBody->collisionGroup, collisionGroup);
}


/**
 * Set collisionGroup to physicsBody.
 */
static inline void APhysicsBody_SetCollisionGroup(PhysicsBody* physicsBody, int collisionGroup)
{
    ABitwise_Set(physicsBody->collisionGroup, collisionGroup);
}


/**
 * Clear collisionGroup in physicsBody.
 */
static inline void APhysicsBody_ClearCollisionGroup(PhysicsBody* physicsBody, int collisionGroup)
{
    ABitwise_Clear(physicsBody->collisionGroup, collisionGroup);
}


#endif
