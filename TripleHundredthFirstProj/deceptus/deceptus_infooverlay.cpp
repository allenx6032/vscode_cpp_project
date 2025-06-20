#include "deceptus_infooverlay.h"

#include "deceptus_easings.h"
#include "deceptus_tmxobject.h"
#include "deceptus_tmxproperties.h"
#include "deceptus_tmxproperty.h"
#include "deceptus_gameconfiguration.h"
#include "deceptus_texturepool.h"
#include "deceptus_gamemechanismdeserializerregistry.h"

// #include <iostream>

namespace
{
const auto registered_infooverlay = []
{
   auto& registry = GameMechanismDeserializerRegistry::instance();
   registry.mapGroupToLayer("InfoOverlay", "info_overlays");

   registry.registerLayerName(
      "info_overlays",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = InfoOverlay::setup(parent, data);
         mechanisms["info_overlays"]->push_back(mechanism);
      }
   );
   registry.registerObjectGroup(
      "InfoOverlay",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = InfoOverlay::setup(parent, data);
         mechanisms["info_overlays"]->push_back(mechanism);
      }
   );
   return true;
}();
}  // namespace

InfoOverlay::InfoOverlay(GameNode* parent) : GameNode(parent)
{
   setClassName(typeid(InfoOverlay).name());
   setEnabled(false);
   setZ(static_cast<int32_t>(ZDepth::ForegroundMax));
}

void InfoOverlay::update(const sf::Time& delta_time)
{
   if (!isEnabled())
   {
      return;
   }

   _elapsed += FloatSeconds(delta_time.asSeconds());

   auto alpha = 0.0f;

   if (_elapsed < _settings._start_delay_duration)
   {
      alpha = 0.0f;
   }
   else if (_elapsed < _settings._start_delay_duration + _settings._fade_in_duration)
   {
      // fade in
      const auto fade_in_start = _settings._start_delay_duration;
      const auto elapsed_normalized = ((_elapsed.count() - fade_in_start.count()) / _settings._fade_in_duration.count());
      alpha = Easings::easeInOutQuad(elapsed_normalized);
   }
   else if (_elapsed < _settings._start_delay_duration + _settings._fade_in_duration + _settings._show_duration)
   {
      // wait
      alpha = 1.0f;
   }
   else if (_elapsed <
            _settings._start_delay_duration + _settings._fade_in_duration + _settings._show_duration + _settings._fade_out_duration)
   {
      // fade out
      const auto fade_out_start = _settings._start_delay_duration + _settings._fade_in_duration + _settings._show_duration;
      const auto elapsed_normalized = ((_elapsed.count() - fade_out_start.count()) / _settings._fade_out_duration.count());
      alpha = 1.0f - Easings::easeInOutQuad(elapsed_normalized);
   }
   else
   {
      // done
      alpha = 0.0f;
      setEnabled(false);

      if (!_settings._show_once)
      {
         _elapsed = FloatSeconds{0};
      }
   }

   // std::cout << alpha << std::endl;

   _sprite->setColor(sf::Color(255, 255, 255, static_cast<uint8_t>(255 * alpha)));
}

void InfoOverlay::draw(sf::RenderTarget& color, sf::RenderTarget& /*normal*/)
{
   if (!isEnabled())
   {
      return;
   }

   const auto& level_view = color.getView();
   if (_settings._fullscreen)
   {
      const sf::View ortho(sf::FloatRect(
         {0.0f, 0.0f},
         {static_cast<float>(GameConfiguration::getInstance()._view_width),
          static_cast<float>(GameConfiguration::getInstance()._view_height)}
      ));

      color.setView(ortho);
   }

   color.draw(*_sprite);

   if (_settings._fullscreen)
   {
      color.setView(level_view);
   }
}

std::shared_ptr<InfoOverlay> InfoOverlay::setup(GameNode* parent, const GameDeserializeData& data)
{
   auto instance = std::make_shared<InfoOverlay>(parent);

   if (data._tmx_object->_properties)
   {
      const auto z = data._tmx_object->_properties->_map.find("z");
      if (z != data._tmx_object->_properties->_map.end())
      {
         instance->_z_index = z->second->_value_int.value();
      }

      const auto start_delay_duration = data._tmx_object->_properties->_map.find("start_delay_duration");
      if (start_delay_duration != data._tmx_object->_properties->_map.end())
      {
         instance->_settings._start_delay_duration = FloatSeconds(start_delay_duration->second->_value_float.value());
      }

      const auto fade_in_duration = data._tmx_object->_properties->_map.find("fade_in_duration");
      if (fade_in_duration != data._tmx_object->_properties->_map.end())
      {
         instance->_settings._fade_in_duration = FloatSeconds(fade_in_duration->second->_value_float.value());
      }

      const auto show_duration = data._tmx_object->_properties->_map.find("show_duration");
      if (show_duration != data._tmx_object->_properties->_map.end())
      {
         instance->_settings._show_duration = FloatSeconds(show_duration->second->_value_float.value());
      }

      const auto fade_out_duration = data._tmx_object->_properties->_map.find("fade_out_duration");
      if (fade_out_duration != data._tmx_object->_properties->_map.end())
      {
         instance->_settings._fade_out_duration = FloatSeconds(fade_out_duration->second->_value_float.value());
      }

      const auto show_once = data._tmx_object->_properties->_map.find("show_once");
      if (show_once != data._tmx_object->_properties->_map.end())
      {
         instance->_settings._show_once = show_once->second->_value_bool.value();
      }

      const auto fullscreen = data._tmx_object->_properties->_map.find("fullscreen");
      if (fullscreen != data._tmx_object->_properties->_map.end())
      {
         instance->_settings._fullscreen = fullscreen->second->_value_bool.value();
      }

      const auto texture_id = data._tmx_object->_properties->_map.find("texture");
      if (texture_id != data._tmx_object->_properties->_map.end())
      {
         instance->_texture = TexturePool::getInstance().get(texture_id->second->_value_string.value());
         instance->_sprite = std::make_unique<sf::Sprite>(*instance->_texture);
      }

      // read texture rect
      sf::IntRect rect;
      const auto texture_rect_x = data._tmx_object->_properties->_map.find("texture_rect_x");
      if (texture_rect_x != data._tmx_object->_properties->_map.end())
      {
         rect.position.x = texture_rect_x->second->_value_int.value();
      }

      const auto texture_rect_y = data._tmx_object->_properties->_map.find("texture_rect_y");
      if (texture_rect_y != data._tmx_object->_properties->_map.end())
      {
         rect.position.y = texture_rect_y->second->_value_int.value();
      }

      const auto texture_rect_width = data._tmx_object->_properties->_map.find("texture_rect_width");
      if (texture_rect_width != data._tmx_object->_properties->_map.end())
      {
         rect.size.x = texture_rect_width->second->_value_int.value();
      }

      const auto texture_rect_height = data._tmx_object->_properties->_map.find("texture_rect_height");
      if (texture_rect_height != data._tmx_object->_properties->_map.end())
      {
         rect.size.y = texture_rect_height->second->_value_int.value();
      }

      if (rect.size.x > 0 && rect.size.y > 0)
      {
         instance->_sprite->setTextureRect(rect);
      }
   }

   const auto bounding_rect =
      sf::FloatRect{{data._tmx_object->_x_px, data._tmx_object->_y_px}, {data._tmx_object->_width_px, data._tmx_object->_height_px}};

   instance->setObjectId(data._tmx_object->_name);
   instance->_rect = bounding_rect;
   instance->_sprite->setColor(sf::Color(255, 255, 255, 0));
   instance->addChunks(bounding_rect);

   if (!instance->_settings._fullscreen)
   {
      instance->_sprite->setPosition({data._tmx_object->_x_px, data._tmx_object->_y_px});
   }

   return instance;
}

std::optional<sf::FloatRect> InfoOverlay::getBoundingBoxPx()
{
   return std::nullopt;
}
