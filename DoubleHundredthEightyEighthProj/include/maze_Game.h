#pragma once
#include "maze_Manager.h"
#include "maze_Menus.h"
#include "maze_Maze.h"
#include "maze_Player.h"
#include "maze_Timer.h"
#include "maze_Window.h"
#include "raylib.h"
#include <utility>

class Game {

public:
  Game(Window *win, float scale);
  ~Game();

  void init();
  void Loop();
  void DrawGame(Rectangle &frameRec);
  void setScale(float scale); // (Deprecated) Set Scale for the Game
  void resetWinState();

private:
  Texture2D darknessTexture;            // darkness texture for drawing the area
                                        // outside the maze
  Vector2 mousePosition = {0.0f, 0.0f}; // Mouse position (Updated Every Frame)
  float scale = 1;
  Window *__window;      //  Window Object Pointer
  Maze *__maze;          //  Maze Object Pointer
  Player *__player;      //  Player Object Pointer
  Manager *__manager;    //  Manager Object Pointer
  Camera2D camera = {0}; // Camera Object for handling the Player View
  Menu *__menu;          // Gui Menu object Pointer
  Timer *__sessionTimer;
};
// comment
