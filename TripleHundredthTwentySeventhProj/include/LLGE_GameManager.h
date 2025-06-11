#ifndef _GameManager
#define _GameManager

#include <vector>
#include <SFML_Graphics.hpp>
#include "LLGE_EntityLuaInterface.h"


class GameManager{

 private:
  sf::Clock ET;

  sf::RenderWindow App;

  EntityLuaInterface ELI;

  Entity* entity;

 public:

  GameManager();

  void gameLoop();
  void update();
  void draw();
  void input();  

  void initialize();

};

#endif 
