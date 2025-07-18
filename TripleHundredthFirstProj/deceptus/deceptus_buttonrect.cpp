#include "deceptus_buttonrect.h"
#include "deceptus_tmxproperties.h"
#include "deceptus_valuereader.h"
#include "deceptus_gamemechanismdeserializerregistry.h"
#include "deceptus_gamemechanismobserver.h"
#include "deceptus_player.h"

namespace
{
const auto registered_buttonrect = []
{
   auto& registry = GameMechanismDeserializerRegistry::instance();
   registry.mapGroupToLayer("ButtonRect", "button_rects");

   registry.registerLayerName(
      "button_rects",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = std::make_shared<ButtonRect>(parent);
         mechanism->setup(data);
         mechanisms["button_rects"]->push_back(mechanism);
      }
   );

   registry.registerObjectGroup(
      "ButtonRect",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = std::make_shared<ButtonRect>(parent);
         mechanism->setup(data);
         mechanisms["button_rects"]->push_back(mechanism);
      }
   );
   return true;
}();
}  // namespace

ButtonRect::ButtonRect(GameNode* parent) : GameNode(parent)
{
   setClassName(typeid(GameNode).name());
}

void ButtonRect::update(const sf::Time& /*dt*/)
{
   _player_intersects = Player::getCurrent()->getPixelRectFloat().findIntersection(_rect).has_value();

   if (!_player_intersects)
   {
      return;
   }

   if ((_botton == Button::A && Player::getCurrent()->getControls()->isButtonAPressed()) ||
       (_botton == Button::B && Player::getCurrent()->getControls()->isButtonBPressed()) ||
       (_botton == Button::X && Player::getCurrent()->getControls()->isButtonXPressed()) ||
       (_botton == Button::Y && Player::getCurrent()->getControls()->isButtonYPressed()))
   {
      GameMechanismObserver::onEvent(getObjectId(), "button_rects", "pressed", "true");
   }
}

std::optional<sf::FloatRect> ButtonRect::getBoundingBoxPx()
{
   return _rect;
}

void ButtonRect::setup(const GameDeserializeData& data)
{
   setObjectId(data._tmx_object->_name);
   _rect = sf::FloatRect{{data._tmx_object->_x_px, data._tmx_object->_y_px}, {data._tmx_object->_width_px, data._tmx_object->_height_px}};

   if (data._tmx_object->_properties)
   {
      static std::unordered_map<std::string, Button> button_map{{"a", Button::A}, {"b", Button::B}, {"x", Button::X}, {"y", Button::Y}};

      const auto& map = data._tmx_object->_properties->_map;
      const auto button_id_str = ValueReader::readValue<std::string>("button", map).value_or("b");
   }

   addChunks(_rect);
}
