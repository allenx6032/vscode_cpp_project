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
#endif

//
// Character animation data
//
const uint8 E_SIZE_CHARACTER_STATES = 2;
enum {
    ECS_IDLE,
    ECS_WALKING
} typedef E_CharacterState;

struct {
    SDL_Rect *spritesRects;
    int count;
} typedef AnimationSprites;


struct {
    E_CharacterState state;
    int frame;
    int nextUpdate;

    SDL_Texture *spritesTexture;
    AnimationSprites *animationsPerState;
} typedef CharacterAnimations;

//
// Character entity data
//
struct {
    int x, y;
    int w, h;
    bool left, right, up, down;

    CharacterAnimations *animations;
} typedef Character;


inline function Character
character_MakePlayer(CharacterAnimations *animations)
{
    Character character = {};
    character.x = 0;
    character.y = 200;
    character.w = 15 * RENDER_SCALE;
    character.h = 22 * RENDER_SCALE;
    character.animations = animations;

    return character;
}

inline function AnimationSprites *
character_GetCurrentAnimationSprites(Character *character)
{
    AnimationSprites *currentSprites = character->animations->animationsPerState;
    for (int i = 0; i < character->animations->state; ++i) {
        ++currentSprites;
    }
    return currentSprites;
}

inline function SDL_Rect *
character_GetAnimationSpriteRect(AnimationSprites *sprites, int frame) 
{
    SDL_Rect *currentRect = sprites->spritesRects;
    for (int i = 0; i < frame; ++i) {
        currentRect++;
    }
    return currentRect;
}

inline function void
character_Update(Character *character)
{
    // State
    E_CharacterState LastState = character->animations->state;

    character->animations->state = ECS_IDLE;
    if (character->left) {
        character->x -= CHARACTER_SPEED;
        character->animations->state = ECS_WALKING;
    }
    if (character->right) {
        character->x += CHARACTER_SPEED;
        character->animations->state = ECS_WALKING;
    }
    if (character->up) {
        character->y -= CHARACTER_SPEED;
        character->animations->state = ECS_WALKING;
    }
    if (character->down) {
        character->y += CHARACTER_SPEED;
        character->animations->state = ECS_WALKING;
    }

    // Changed state?
    if (LastState != character->animations->state) {
        character->animations->nextUpdate = ANIMATION_NEXT_UPDATE;
        character->animations->frame = 0;
    } else {   
        --character->animations->nextUpdate;
        if (character->animations->nextUpdate == 0) {
            character->animations->nextUpdate = ANIMATION_NEXT_UPDATE;
            AnimationSprites *currentSprites = character_GetCurrentAnimationSprites(character);
            character->animations->frame = (character->animations->frame + 1) % currentSprites->count;
        }
    }
}

inline function SDL_Rect *
character_GetCurrentSprite(Character *character)
{
    AnimationSprites *currentSprites = character_GetCurrentAnimationSprites(character);
    SDL_Rect *currentRect = character_GetAnimationSpriteRect(currentSprites, character->animations->frame);
    return currentRect;
}

inline function void
character_ProcessKeyboardEvents(Character *player, SDL_Event *event)
{
    switch(event->type) {
    case SDL_KEYDOWN:
        switch(event->key.keysym.sym) {
            case SDLK_LEFT:
                player->left = 1;
                break;
            case SDLK_RIGHT:
                player->right = 1;
                break;
            case SDLK_UP:
                player->up = 1;
                break;
            case SDLK_DOWN:
                player->down = 1;
                break;
        }
        break;
    case SDL_KEYUP:
        switch(event->key.keysym.sym) {
            case SDLK_LEFT:
                player->left = 0;
                break;
            case SDLK_RIGHT:
                player->right = 0;
                break;
            case SDLK_UP:
                player->up = 0;
                break;
            case SDLK_DOWN:
                player->down = 0;
                break;
        }
        break;
    }
}


//
// Character resources
//
inline function CharacterAnimations *
resources_LoadCharacterAnimations(Arena *arena, SDL_Renderer *renderer)
{
    assert(E_SIZE_CHARACTER_STATES == 2);
    assert(ECS_IDLE == 0);
    assert(ECS_WALKING == 1);

    const int IDLE_COUNT = 1;
    SDL_Rect *idleSpritesRects = (SDL_Rect *) ReserveMemory(arena, sizeof(SDL_Rect) * IDLE_COUNT);
    SDL_Rect *currentIdleSprite = idleSpritesRects;
    *idleSpritesRects = (SDL_Rect) { 9, 42, 15, 22};

    const int WALKING_COUNT = 4;
    SDL_Rect *walkingSpritesRects = (SDL_Rect *) ReserveMemory(arena, sizeof(SDL_Rect) * WALKING_COUNT);
    SDL_Rect *currentWalkingSprite = walkingSpritesRects;
    *currentWalkingSprite = (SDL_Rect) { 41, 41, 15, 22};
    *(++currentWalkingSprite) = (SDL_Rect) {72, 42, 16, 22};
    *(++currentWalkingSprite) = (SDL_Rect) {104, 41, 17, 22};
    *(++currentWalkingSprite) = (SDL_Rect) {9, 42, 15, 22};

    
    AnimationSprites *sprites = (AnimationSprites *) ReserveMemory(arena, sizeof(AnimationSprites) * E_SIZE_CHARACTER_STATES);
    AnimationSprites *currentSprite = sprites;
    currentSprite->spritesRects = idleSpritesRects;
    currentSprite->count = IDLE_COUNT;

    ++currentSprite;
    currentSprite->spritesRects = walkingSpritesRects;
    currentSprite->count = WALKING_COUNT;

    CharacterAnimations *anim = (CharacterAnimations *) ReserveMemory(arena, sizeof(CharacterAnimations));
    anim->state = ECS_IDLE;
    anim->frame = 0;
    anim->nextUpdate = ANIMATION_NEXT_UPDATE;
    anim->animationsPerState = sprites;
    anim->spritesTexture = IMG_LoadTexture(renderer, "res/characters.png");

    return anim;
}
