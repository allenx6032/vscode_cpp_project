#include "deceptus_damagerect.h"

#include "deceptus_tmxproperties.h"
#include "deceptus_tmxproperty.h"
#include "deceptus_gamemechanismdeserializerregistry.h"
#include "deceptus_player.h"

namespace
{
const auto registered_damagerect = []
{
   auto& registry = GameMechanismDeserializerRegistry::instance();

   registry.mapGroupToLayer("DamageRect", "damage_rects");

   registry.registerLayerName(
      "damage_rects",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = std::make_shared<DamageRect>(parent);
         mechanism->setup(data);
         mechanisms["damage_rects"]->push_back(mechanism);
      }
   );

   registry.registerObjectGroup(
      "damage_rect",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = std::make_shared<DamageRect>(parent);
         mechanism->setup(data);
         mechanisms["damage_rects"]->push_back(mechanism);
      }
   );
   return true;
}();
}  // namespace

DamageRect::DamageRect(GameNode* /*parent*/)
{
   setClassName(typeid(GameNode).name());
}

void DamageRect::update(const sf::Time& /*dt*/)
{
   auto* player = Player::getCurrent();
   const auto player_intersects = player->getPixelRectFloat().findIntersection(_rect).has_value();

   if (player_intersects)
   {
      player->damage(_damage);
   }
}

std::optional<sf::FloatRect> DamageRect::getBoundingBoxPx()
{
   return _rect;
}

void DamageRect::setup(const GameDeserializeData& data)
{
   _rect = sf::FloatRect{{data._tmx_object->_x_px, data._tmx_object->_y_px}, {data._tmx_object->_width_px, data._tmx_object->_height_px}};

   if (data._tmx_object->_properties)
   {
      auto damage_it = data._tmx_object->_properties->_map.find("damage");
      if (damage_it != data._tmx_object->_properties->_map.end())
      {
         _damage = damage_it->second->_value_int.value();
      }
   }
}
