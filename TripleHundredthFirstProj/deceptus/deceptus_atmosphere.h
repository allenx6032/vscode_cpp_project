#pragma once

#include "deceptus_constants.h"
#include "deceptus_tilemap.h"

#include <b2_box2d.h>
#include <SFML_Graphics.hpp>

#include <cstdint>
#include <vector>

struct TmxLayer;
struct TmxTileSet;

struct Atmosphere
{
   Atmosphere() = default;
   ~Atmosphere();

   void parse(const std::shared_ptr<TmxLayer>& layer, const std::shared_ptr<TmxTileSet>& tileSet);
   AtmosphereTile getTileForPosition(const b2Vec2& pos_m) const;
   AtmosphereTile getTileForPosition(const sf::Vector2f& pos_px) const;

   std::vector<int32_t> _map;

   int32_t _map_offset_x_px = 0;
   int32_t _map_offset_y_py = 0;
   int32_t _map_width_tl = 0;
   int32_t _map_height_tl = 0;

   std::vector<std::vector<sf::Vertex>> _outlines;
   std::shared_ptr<TileMap> _tile_map;
};
