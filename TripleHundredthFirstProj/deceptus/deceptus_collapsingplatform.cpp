#include "deceptus_collapsingplatform.h"

#include <iostream>

#include "deceptus_audio.h"
#include "deceptus_constants.h"
#include "deceptus_tmxobject.h"
#include "deceptus_tmxproperties.h"
#include "deceptus_tmxproperty.h"
#include "deceptus_globalclock.h"
#include "deceptus_log.h"
#include "deceptus_texturepool.h"
#include "deceptus_valuereader.h"
#include "deceptus_gamemechanismdeserializerregistry.h"
#include "deceptus_player.h"

namespace
{
const auto registered_collapsingplatform = []
{
   auto& registry = GameMechanismDeserializerRegistry::instance();
   registry.mapGroupToLayer("CollapsingPlatform", "collapsing_platforms");

   registry.registerLayerName(
      "collapsing_platforms",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = std::make_shared<CollapsingPlatform>(parent, data);
         mechanisms["collapsing_platforms"]->push_back(mechanism);
      }
   );

   registry.registerObjectGroup(
      "CollapsingPlatform",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = std::make_shared<CollapsingPlatform>(parent, data);
         mechanisms["collapsing_platforms"]->push_back(mechanism);
      }
   );
   return true;
}();
}  // namespace

namespace
{
constexpr auto sprite_column_count = 38;
constexpr auto bevel_m = 4 * MPP;
constexpr auto sprite_offset_y_px = -24;
}  // namespace

// animation info
//
// row 0:    trigger state
// row 1:    appear state
// row 2-6:  collapse state

CollapsingPlatform::CollapsingPlatform(GameNode* parent, const GameDeserializeData& data) : FixtureNode(parent)
{
   setClassName(typeid(CollapsingPlatform).name());
   setType(ObjectTypeCollapsingPlatform);
   setObjectId(data._tmx_object->_name);

   if (!data._tmx_object->_properties)
   {
      return;
   }

   const auto& map = data._tmx_object->_properties->_map;

   // read properties
   auto readFloatProperty = [&map](float& value, const std::string& id)
   {
      const auto it = map.find(id);
      if (it != map.end())
      {
         value = it->second->_value_float.value();
      }
   };

   readFloatProperty(_settings.time_to_collapse_s, "time_to_collapse_s");
   readFloatProperty(_settings.destruction_speed, "destruction_speed");
   readFloatProperty(_settings.fall_speed, "fall_speed");
   readFloatProperty(_settings.time_to_respawn_s, "time_to_respawn_s");
   readFloatProperty(_settings.fade_in_duration_s, "fade_in_duration_s");
   setZ(ValueReader::readValue<int32_t>("z", map).value_or(0));

   // set up shape
   //
   //       0        5
   //       +--------+
   //      /          \
   //   1 +            + 4
   //     |            |
   //   2 +------------+ 3

   _width_m = data._tmx_object->_width_px * MPP;
   _height_m = data._tmx_object->_height_px * MPP;

   if (_width_m < 0.01f || _height_m < 0.01f)
   {
      Log::Error() << "collapsing platform has invalid dimensions, object id: " << data._tmx_object->_id;
      return;
   }

   _width_tl = static_cast<int32_t>(data._tmx_object->_width_px / PIXELS_PER_TILE);

   _blocks.resize(_width_tl);

   std::array<b2Vec2, 7> vertices{
      b2Vec2{bevel_m, 0.0f},
      b2Vec2{0.0f, bevel_m},
      b2Vec2{0.0f, _height_m},
      b2Vec2{_width_m, _height_m},
      b2Vec2{_width_m, bevel_m},
      b2Vec2{_width_m - bevel_m, 0.0f},
      b2Vec2{bevel_m, 0.0f},
   };

   _shape.Set(vertices.data(), static_cast<int32_t>(vertices.size()));

   // create body
   const auto x = data._tmx_object->_x_px;
   const auto y = data._tmx_object->_y_px;
   _position_m = MPP * b2Vec2{x, y};
   _position_px = sf::Vector2f(x, y);
   _rect_px = {{x, y}, {data._tmx_object->_width_px, data._tmx_object->_height_px}};

   addChunks(_rect_px);

   b2BodyDef body_def;
   body_def.type = b2_staticBody;
   body_def.position = _position_m;
   _body = data._world->CreateBody(&body_def);

   // set up body fixture
   b2FixtureDef fixture_def;
   fixture_def.shape = &_shape;
   fixture_def.density = 1.0f;
   fixture_def.isSensor = false;
   _fixture = _body->CreateFixture(&fixture_def);
   _fixture->SetUserData(static_cast<void*>(this));

   // set up visualization
   _texture = TexturePool::getInstance().get("data/sprites/collapsing_platform.png");

   // initialize all blocks
   auto sprite_offset_x_px = 0;
   auto row_index = 0;
   for (auto& block : _blocks)
   {
      block._sprite = std::make_unique<sf::Sprite>(*_texture);
      block._x_px = x + sprite_offset_x_px;
      block._y_px = y + sprite_offset_y_px;
      block._sprite_row = row_index % 4;
      block._fall_speed = 1.0f + (std::rand() % 256) / 256.0f;
      block._destruction_speed = 1.0f + (std::rand() % 256) / 256.0f;

      sprite_offset_x_px += PIXELS_PER_TILE;
      row_index++;
   }
}

void CollapsingPlatform::preload()
{
   Audio::getInstance().addSample("mechanism_collapsing_platform_crumble.wav");
}

void CollapsingPlatform::draw(sf::RenderTarget& color, sf::RenderTarget& /*normal*/)
{
   if (_blocks.empty() || _blocks[0]._sprite_column == sprite_column_count)
   {
      return;
   }

   for (auto& block : _blocks)
   {
      color.draw(*block._sprite);
   }
}

void CollapsingPlatform::updateRespawnAnimation()
{
   if (!_respawning)
   {
      return;
   }

   if (_time_since_collapse.asSeconds() > _settings.time_to_respawn_s + _settings.fade_in_duration_s)
   {
      for (auto& block : _blocks)
      {
         block._sprite->setColor(sf::Color{255, 255, 255, 255});
      }

      _respawning = false;
      _collapsed = false;
      _body->SetEnabled(true);
   }
   else
   {
      auto alpha_normalized = std::min(_time_since_collapse.asSeconds() - _settings.time_to_respawn_s, _settings.fade_in_duration_s) /
                              _settings.fade_in_duration_s;

      // std::cout << alpha_normalized << std::endl;

      for (auto& block : _blocks)
      {
         block._alpha = static_cast<uint8_t>(255.0f * alpha_normalized);
         block._sprite->setColor(sf::Color{255, 255, 255, block._alpha});
      }
   }
}

void CollapsingPlatform::updateRespawn(const sf::Time& dt)
{
   _time_since_collapse += dt;

   // bring collapsed blocks back after some time
   if (!_respawning && _time_since_collapse.asSeconds() > _settings.time_to_respawn_s)
   {
      if (Player::getCurrent()->getPixelRectFloat().findIntersection(_rect_px).has_value())
      {
         // shift respawn time while player intersects
         _time_since_collapse = sf::seconds(_settings.time_to_respawn_s);
      }
      else
      {
         _respawning = true;
         resetAllBlocks();
      }
   }
}

void CollapsingPlatform::updateShakeBlocks()
{
   constexpr auto frequency = 40.0f;
   constexpr auto amplitude = 2.0f;
   auto offset = 0.0f;
   for (auto& block : _blocks)
   {
      block._shake_y_px = amplitude * sin(offset + frequency * _elapsed_s);
      offset += 1.0f;
   }
}

void CollapsingPlatform::collapse()
{
   if (_collapsed)
   {
      return;
   }

   _foot_sensor_contact = false;
   _collapsed = true;
   _time_since_collapse = {};

   // disable the body so the player falls through
   _body->SetEnabled(false);
}

void CollapsingPlatform::updateBlockDestruction(const sf::Time& dt)
{
   if (_respawning)
   {
      return;
   }

   if (!_collapsed)
   {
      return;
   }

   const auto dt_s = dt.asSeconds();
   for (auto& block : _blocks)
   {
      block._elapsed_s += dt_s;
      block._sprite_column =
         std::min(static_cast<int32_t>(block._elapsed_s * _settings.destruction_speed * block._destruction_speed), sprite_column_count);
      block._fall_offset_y_px = block._elapsed_s * block._fall_speed * _settings.fall_speed;
   }
}

void CollapsingPlatform::resetAllBlocks()
{
   for (auto& block : _blocks)
   {
      block.reset();
   }
}

void CollapsingPlatform::update(const sf::Time& dt)
{
   if (!_body)
   {
      return;
   }

   _elapsed_s += dt.asSeconds();

   if (_foot_sensor_contact)
   {
      if (!_played_shake_sample)
      {
         Audio::getInstance().playSample({"mechanism_collapsing_platform_crumble.wav"});
         _played_shake_sample = true;
      }

      _collapse_elapsed_s += dt.asSeconds();
      if (_collapse_elapsed_s > _settings.time_to_collapse_s)
      {
         collapse();
      }
      else
      {
         // tiles should be shaking a bit
         updateShakeBlocks();
      }
   }
   else
   {
      if (_played_shake_sample)
      {
         Audio::getInstance().stopSample("mechanism_collapsing_platform_crumble.wav");
         _played_shake_sample = false;
      }

      // keep resetting collapse time while there are no feet on the platform
      _collapse_elapsed_s = 0.0f;

      if (!_collapsed)
      {
         // player left the platform before it collapsed
         resetAllBlocks();
      }
      else
      {
         // measure time since collapse
         updateRespawn(dt);
         updateRespawnAnimation();
      }
   }

   updateBlockDestruction(dt);
   updateBlockSprites();
}

std::optional<sf::FloatRect> CollapsingPlatform::getBoundingBoxPx()
{
   return _rect_px;
}

void CollapsingPlatform::beginContact(b2Contact* /*contact*/, FixtureNode* other)
{
   if (other->getType() != ObjectTypePlayerFootSensor)
   {
      return;
   }

   _foot_sensor_contact = true;
}

void CollapsingPlatform::endContact(FixtureNode* other)
{
   if (other->getType() != ObjectTypePlayerFootSensor)
   {
      return;
   }

   _foot_sensor_contact = false;
}

void CollapsingPlatform::updateBlockSprites()
{
   for (auto& block : _blocks)
   {
      block._sprite->setPosition({block._x_px + block._shake_x_px, block._y_px + block._shake_y_px + block._fall_offset_y_px});
      block._sprite->setTextureRect(
         {{block._sprite_column * PIXELS_PER_TILE, block._sprite_row * PIXELS_PER_TILE * 3}, {PIXELS_PER_TILE, PIXELS_PER_TILE * 3}}
      );
   }
}
