#pragma once

#include "deceptus_layer.h"
#include "deceptus_layerdata.h"
#include "deceptus_ingamemenupage.h"
#include "deceptus_bitmapfont.h"

#include <SFML_Graphics.hpp>
#include <SFML_System.hpp>

#include <filesystem>
#include <memory>
#include <vector>

class GameMechanism;

class IngameMenuMap : public InGameMenuPage
{
public:
   IngameMenuMap();

   void loadLevelTextures(const std::filesystem::path& grid, const std::filesystem::path& outlines);

   void draw(sf::RenderTarget& window, sf::RenderStates = sf::RenderStates::Default) override;
   void update(const sf::Time& dt) override;

   void setDoors(const std::vector<std::shared_ptr<GameMechanism>>& doors);
   void setPortals(const std::vector<std::shared_ptr<GameMechanism>>& portals);

private:
   void drawLevelItems(sf::RenderTarget& window, sf::RenderStates = sf::RenderStates::Default);
   void updateButtons();
   void updateShowHide();
   void updateMove();

   BitmapFont _font;

   std::unique_ptr<sf::RenderTexture> _level_render_texture;

   std::shared_ptr<sf::Texture> _level_grid_texture;
   std::unique_ptr<sf::Sprite> _level_grid_sprite;

   std::shared_ptr<sf::Texture> _level_outline_texture;
   std::unique_ptr<sf::Sprite> _level_outline_sprite;

   std::vector<std::shared_ptr<GameMechanism>> _doors;
   std::vector<std::shared_ptr<GameMechanism>> _portals;

   std::vector<LayerData> _panel_header;
   std::vector<LayerData> _panel_left;
   std::vector<LayerData> _panel_center;
   std::vector<LayerData> _panel_right;
   std::vector<LayerData> _panel_background;

   FloatSeconds _duration_show;
   FloatSeconds _duration_hide;

   float _zoom = 1.0f;
};
