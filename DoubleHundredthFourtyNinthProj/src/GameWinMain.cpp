//-----------------------------------------------------------------
// Game Engine WinMain Function
// C++ Source - GameWinMain.cpp - version v7_01
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "GameWinMain.h"
#include "GameEngine.h"

#include "DungeonGeneratorMain.h"	

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------

#define GAME_ENGINE (GameEngine::GetSingleton())

//-----------------------------------------------------------------
// Windows Functions
//-----------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (GAME_ENGINE == nullptr) return FALSE;		// create the game engine object, exit if it fails

	GAME_ENGINE->SetGame(new DungeonGeneratorMain());					// any class that implements AbstractGame

	return GAME_ENGINE->Run(hInstance, nCmdShow);	// here we go
}
