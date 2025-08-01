// Copyright (C) 2011-2023 Valeriano Alfonso Rodriguez (Kableado)

#ifndef Input_H
#define Input_H

#include "GameLib_Util.h"

/////////////////////////////
// Input_Init
//
// Initializes the game input.
int Input_Init();

/////////////////////////////
// Input_Frame
//
// Notify a frame update to the input subsystem.
void Input_Frame();

/////////////////////////////
// Input_PostFrame
//
// Notify a frame update end to the input subsystem.
void Input_PostFrame();

////////////////////////////////////////////////
// InputKey //
//////////////
// Key enumeration.
typedef enum {
	InputKey_Action1,
	InputKey_Action2,
	InputKey_Up,
	InputKey_Down,
	InputKey_Left,
	InputKey_Right,
	InputKey_Jump,
	InputKey_Continue,
	InputKey_Exit,

	InputKey_DumpProfiling,
	InputKey_Screenshot,

	InputKey_Max
} InputKey;

/////////////////////////////
// Input_SetKey
//
// Notify a key press to the input subsystem.
void Input_SetKey(InputKey key, int status);

////////////////////////////////////////////////
// InputKeyStatus //
///////////////////
// Key status enumeration.
typedef enum { InputKey_Released, InputKey_Pressed, InputKey_Holded } InputKeyStatus;

/////////////////////////////
// Input_GetKey
//
// Reports a the status of a key.
InputKeyStatus Input_GetKey(InputKey key);

/////////////////////////////
// Input_SetPointerPosition
//
void Input_SetPointerPosition(float x, float y);

/////////////////////////////
// Input_SetPointerDown
//
void Input_SetPointerDown(int pointerDown);

/////////////////////////////
// Input_GetPointerDown
//
int Input_GetPointerDown();

/////////////////////////////
// Input_GetPointerPosition
//
int Input_GetPointerPosition(vec2 pointer);

/////////////////////////////
// Input_GetClickedPosition
//
int Input_GetClickedPosition(vec2 clickPosition);

/////////////////////////////
// Input_AnyKey
//
//
int Input_AnyKey();

/////////////////////////////
// Input_GetDir
//
// Reports the direction of the dpad and mouse.
int Input_GetDir(vec2 dir);

/////////////////////////////
// Input_GetKeyDir
//
// Reports the direction of the dpad.
int Input_GetKeyDir(vec2 dir);

#endif
