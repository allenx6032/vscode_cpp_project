#include "deceptus_menuscreenachievements.h"

#include "deceptus_menu.h"

MenuScreenAchievements::MenuScreenAchievements()
{
   setFilename("data/menus/achievements.psd");
}

void MenuScreenAchievements::up()
{
}

void MenuScreenAchievements::down()
{
}

void MenuScreenAchievements::select()
{
}

void MenuScreenAchievements::back()
{
   Menu::getInstance()->show(Menu::MenuType::Options);
}

void MenuScreenAchievements::keyboardKeyPressed(sf::Keyboard::Key key)
{
   if (key == sf::Keyboard::Key::Up)
   {
      up();
   }

   else if (key == sf::Keyboard::Key::Down)
   {
      down();
   }

   else if (key == sf::Keyboard::Key::Enter)
   {
      select();
   }

   else if (key == sf::Keyboard::Key::Escape)
   {
      back();
   }
}

void MenuScreenAchievements::loadingFinished()
{
   updateLayers();
}

void MenuScreenAchievements::updateLayers()
{
   _layers["back_xbox_0"]->_visible = isControllerUsed();
   _layers["back_xbox_1"]->_visible = false;

   _layers["back_pc_0"]->_visible = !isControllerUsed();
   _layers["back_pc_1"]->_visible = false;
}

/*
data/menus/achievements.psd
    bg_tmp
    back_xbox_0
    back_xbox_1
    back_pc_0
    back_pc_1
    title
*/
