#include "GE_pch.h"

#include "GE_Game.h"

int main(int argc, char* argv[]) 
{
    Game game;

    game.Initialize();
    game.Run();
    game.Destroy();
    return 0;
} 