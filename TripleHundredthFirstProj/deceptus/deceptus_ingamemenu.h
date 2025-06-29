#pragma once

#include "deceptus_gamecontrollerinfo.h"

#include "deceptus_ingamemenuarchives.h"
#include "deceptus_ingamemenuaudio.h"
#include "deceptus_ingamemenuinventory.h"
#include "deceptus_ingamemenumap.h"

#include <SFML_Graphics.hpp>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

struct InventoryItem;

class InGameMenu
{
public:
   enum class SubMenu
   {
      Map = 0,
      Inventory = 1,
      Archives = 2,
   };

   using AudioCallback = std::function<void(InGameMenuAudio::SoundEffect)>;

   InGameMenu();

   void draw(sf::RenderTarget& window, sf::RenderStates = sf::RenderStates::Default);
   void update(const sf::Time& dt);

   void processEvent(const sf::Event::KeyPressed* event);

   void open();
   void close();

   void left();
   void right();
   void up();
   void down();

   void show();
   void hide();

   void setJoystickInfo(const GameControllerInfo& joystickInfo);
   void setAudioCallback(const AudioCallback&);

private:
   void initializeController();
   void updateController();

   void goToRightSubMenu();
   void goToLeftSubMenu();
   void debug();

   void updateControllerActions();
   bool isControllerActionSkipped() const;

   void rotateRight();
   void rotateLeft();

   GameControllerInfo _joystick_info;
   float _joystick_update_time = 0.0f;

   std::shared_ptr<InGameMenuArchives> _menu_archives;
   std::shared_ptr<InGameMenuInventory> _menu_inventory;
   std::shared_ptr<IngameMenuMap> _menu_map;

   SubMenu _selected_submenu{SubMenu::Inventory};
   std::optional<SubMenu> _previous_submenu;

   std::array<SubMenu, 3> _submenu_selection;
   std::array<std::shared_ptr<InGameMenuPage>, 3> _submenu_type_map;
   std::array<std::string, 3> _submenu_type_names;

   InGameMenuAudio _audio;
   AudioCallback _audio_callback;
};
