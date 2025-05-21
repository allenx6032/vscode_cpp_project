#include "maze_Game.h"
#include "maze_Window.h"
#include "ini.h"
#include <maze_Maze.h>
#include <raylib.h>
// #include "ini.h"
//
//
#include <cstdlib> // For std::rand() and std::srand
#include <ctime>

int main() {

  std::srand(std::time(0));
  Window *window = new Window(1280, 720, "The Explorer | Puzzle Game");
  // Maze maze(60, 40);
  SetExitKey(KEY_NULL);
  InitAudioDevice(); // Initialize audio device

  Game game(window, 1);

  game.Loop();

  return 0;
}
