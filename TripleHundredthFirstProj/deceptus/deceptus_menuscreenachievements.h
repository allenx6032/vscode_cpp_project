#pragma once

#include "deceptus_menuscreen.h"

class MenuScreenAchievements : public MenuScreen
{
public:
   MenuScreenAchievements();
   void keyboardKeyPressed(sf::Keyboard::Key key) override;

   void loadingFinished() override;
   void updateLayers();

   void up();
   void down();
   void select();
   void back();
};
