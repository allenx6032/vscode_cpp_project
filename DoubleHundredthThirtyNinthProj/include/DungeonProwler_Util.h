//-------------------------------------------------------------------------------------
// Util.h
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Written by Dale Green. Copyright (c) Packt Publishing. All rights reserved.
//-------------------------------------------------------------------------------------
#ifndef UTIL_H
#define UTIL_H

// Game states.
enum class GAME_STATE {
	MAIN_MENU,
	PLAYING,
	GAME_OVER
};

// Spawnable items.
enum class ITEM {
	GEM,
	GOLD,
	HEART,
	POTION,
	KEY,
	COUNT
};

// Enemy types.
enum class ENEMY {
	SLIME,
	HUMANOID,
	COUNT
};

// Enemy humanoid types.
enum class HUMANOID {
	GOBLIN,
	SKELETON,
	COUNT
};

// Animation states.
enum class ANIMATION_STATE {
	WALK_UP,
	WALK_DOWN,
	WALK_RIGHT,
	WALK_LEFT,
	IDLE_UP,
	IDLE_DOWN,
	IDLE_RIGHT,
	IDLE_LEFT,
	COUNT
};

// Tiles.
enum class TILE {
	WALL_SINGLE,
	WALL_TOP_END,
	WALL_SIDE_RIGHT_END,
	WALL_BOTTOM_LEFT,
	WALL_BOTTOM_END,
	WALL_SIDE,
	WALL_TOP_LEFT,
	WALL_SIDE_LEFT_T,
	WALL_SIDE_LEFT_END,
	WALL_BOTTOM_RIGHT,
	WALL_TOP,
	WALL_BOTTOM_T,
	WALL_TOP_RIGHT,
	WALL_SIDE_RIGHT_T,
	WALL_TOP_T,
	WALL_INTERSECTION,
	WALL_DOOR_LOCKED,
	WALL_DOOR_UNLOCKED,
	WALL_ENTRANCE,
	FLOOR,
	FLOOR_ALT,
	EMPTY,
	COUNT
};

// Game views.
enum class VIEW {
	MAIN,
	UI,
	COUNT
};

// Player classes.
enum class PLAYER_CLASS {
	WARRIOR,
	MAGE,
	ARCHER,
	THIEF,
	COUNT
};

// Player traits.
enum class PLAYER_TRAIT {
    ATTACK,
    DEFENSE,
    STRENGTH,
    DEXTERITY,
    STAMINA,
    COUNT
};

// Potions.
enum class POTION {
    ATTACK,
    DEFENSE,
    STRENGTH,
    DEXTERITY,
    STAMINA,
    COUNT
};

// Colors provided by SFML.
enum class COLOR {
	BLACK,
	WHITE,
	RED,
	GREEN,
	BLUE,
	YELLOW,
	MAGENTA,
	CYAN,
	TRANSPARENT,
	COUNT
};

// Music tracks.
enum class MUSIC_TRACK {
    ALT_1,
    ALT_2,
    ALT_3,
    ALT_4,
    ALT_5,
    ALT_6,
    COUNT
};

// Ambient sound effects
enum class AMBIENT_SOUND {
    OWL_HOOT,
    WATER_DRIP,
    CRICKETS,
    COUNT
};
#endif