#include "deceptus_staticlight.h"

#include "deceptus_fbm.h"
#include "deceptus_tmxobject.h"
#include "deceptus_tmxobjectgroup.h"
#include "deceptus_tmxproperties.h"
#include "deceptus_tmxproperty.h"
#include "deceptus_tmxtools.h"
#include "deceptus_globalclock.h"
#include "deceptus_texturepool.h"
#include "deceptus_gamemechanismdeserializerregistry.h"

#include <array>
#include <filesystem>

namespace
{
const auto registered_staticlight = []
{
   auto& registry = GameMechanismDeserializerRegistry::instance();
   registry.mapGroupToLayer("StaticLight", "static_lights");

   registry.registerLayerName(
      "static_lights",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = std::make_shared<StaticLight>(parent);
         mechanism->deserialize(data);
         mechanisms["static_lights"]->push_back(mechanism);
      }
   );
   registry.registerObjectGroup(
      "StaticLight",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = std::make_shared<StaticLight>(parent);
         mechanism->deserialize(data);
         mechanisms["static_lights"]->push_back(mechanism);
      }
   );
   return true;
}();
}  // namespace

namespace
{
auto instance_count = 0;
}

StaticLight::StaticLight(GameNode* parent) : GameNode(parent), _instance_number(instance_count++)
{
   setClassName(typeid(StaticLight).name());
}

void StaticLight::draw(sf::RenderTarget& target, sf::RenderTarget& /*color*/)
{
   auto lumen = fbm::mix(_color.a, _flicker_amount * 255.0f, 1.0f - _flicker_alpha_amount);

   sf::Color color{_color.r, _color.g, _color.b, static_cast<uint8_t>(lumen)};

   _sprite->setColor(color);
   target.draw(*_sprite, _blend_mode);
}

void StaticLight::update(const sf::Time& /*time*/)
{
   const auto time = GlobalClock::getInstance().getElapsedTime();

   if (_flicker_intensity > 0.0001f)
   {
      _flicker_amount = _flicker_intensity * fbm::fbm(fbm::vec2{
                                                _instance_number + time.asSeconds() * _flicker_speed,
                                                _time_offset + _instance_number / static_cast<float>(instance_count)
                                             });
   }
}

std::optional<sf::FloatRect> StaticLight::getBoundingBoxPx()
{
   return std::nullopt;
}

void StaticLight::deserialize(const GameDeserializeData& data)
{
   setObjectId(data._tmx_object->_name);

   std::array<uint8_t, 4> rgba = {255, 255, 255, 255};
   std::string texture = "data/light/smooth.png";
   auto flicker_intensity = 0.0f;
   auto flicker_alpha_amount = 1.0f;
   auto flicker_speed = 0.0f;

   if (data._tmx_object->_properties)
   {
      auto it = data._tmx_object->_properties->_map.find("color");
      if (it != data._tmx_object->_properties->_map.end())
      {
         rgba = TmxTools::color(it->second->_value_string.value());
      }

      it = data._tmx_object->_properties->_map.find("texture");
      if (it != data._tmx_object->_properties->_map.end())
      {
         texture = (std::filesystem::path("data/light/") / it->second->_value_string.value()).string();
      }

      it = data._tmx_object->_properties->_map.find("flicker_intensity");
      if (it != data._tmx_object->_properties->_map.end())
      {
         flicker_intensity = it->second->_value_float.value();
      }

      it = data._tmx_object->_properties->_map.find("flicker_alpha_amount");
      if (it != data._tmx_object->_properties->_map.end())
      {
         flicker_alpha_amount = it->second->_value_float.value();
      }

      it = data._tmx_object->_properties->_map.find("flicker_speed");
      if (it != data._tmx_object->_properties->_map.end())
      {
         flicker_speed = it->second->_value_float.value();
      }

      it = data._tmx_object->_properties->_map.find("z");
      if (it != data._tmx_object->_properties->_map.end())
      {
         _z_index = it->second->_value_int.value();
      }
   }

   _color.r = rgba[0];
   _color.g = rgba[1];
   _color.b = rgba[2];
   _color.a = rgba[3];
   _flicker_intensity = flicker_intensity;
   _flicker_alpha_amount = flicker_alpha_amount;
   _flicker_speed = flicker_speed;
   _texture = TexturePool::getInstance().get(texture);
   _sprite = std::make_unique<sf::Sprite>(*_texture);
   _sprite->setColor(_color);
   _sprite->setPosition({data._tmx_object->_x_px, data._tmx_object->_y_px});

   _rect = sf::FloatRect{{data._tmx_object->_x_px, data._tmx_object->_y_px}, {data._tmx_object->_width_px, data._tmx_object->_height_px}};
   addChunks(_rect);

   auto scale_x_px = data._tmx_object->_width_px / _texture->getSize().x;
   auto scale_y_px = data._tmx_object->_height_px / _texture->getSize().y;
   _sprite->scale({scale_x_px, scale_y_px});

   // init each light with a different time offset
   // probably passing the position itself to FBM would be enough
   std::srand(static_cast<uint32_t>(data._tmx_object->_x_px * data._tmx_object->_y_px));
   _time_offset = (std::rand() % 100) * 0.01f;
}
