#include "deceptus_dust.h"

#include "deceptus_fbm.h"
#include "deceptus_tmxobject.h"
#include "deceptus_tmxproperties.h"
#include "deceptus_tmxproperty.h"
#include "deceptus_tmxtools.h"
#include "deceptus_texturepool.h"
#include "deceptus_gamemechanismdeserializerregistry.h"

namespace
{
const auto registered_dust = []
{
   auto& registry = GameMechanismDeserializerRegistry::instance();
   registry.mapGroupToLayer("Dust", "dust");

   registry.registerLayerName(
      "dust",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = Dust::deserialize(parent, data);
         mechanisms["dust"]->push_back(mechanism);
      }
   );
   registry.registerObjectGroup(
      "Dust",
      [](GameNode* parent, const GameDeserializeData& data, auto& mechanisms)
      {
         auto mechanism = Dust::deserialize(parent, data);
         mechanisms["dust"]->push_back(mechanism);
      }
   );
   return true;
}();
}  // namespace

Dust::Dust(GameNode* parent) : GameNode(parent)
{
   setClassName(typeid(Dust).name());
   _vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
}

void Dust::update(const sf::Time& dt)
{
   const auto dt_s = dt.asSeconds();

   const auto scale_factor_x = static_cast<float>(_flow_field_image.getSize().x) / _clip_rect.size.x;
   const auto scale_factor_y = static_cast<float>(_flow_field_image.getSize().y) / _clip_rect.size.y;

   for (auto& p : _particles)
   {
      const auto x_px = p._position.x - _clip_rect.position.x;
      const auto y_px = p._position.y - _clip_rect.position.y;

      if (x_px < 0 || x_px >= _clip_rect.size.x || y_px < 0 || y_px >= _clip_rect.size.y)
      {
         p.spawn(_clip_rect);
         continue;
      }

      const auto col =
         _flow_field_image.getPixel({static_cast<uint32_t>(x_px * scale_factor_x), static_cast<uint32_t>(y_px * scale_factor_y)});
      const auto col_x = (static_cast<float>(col.r) / 255.0f) - 0.5f;
      const auto col_y = (static_cast<float>(col.g) / 255.0f) - 0.5f;
      const auto col_z = (static_cast<float>(col.b) / 255.0f) - 0.5f;
      const auto dir = sf::Vector3f{col_x, col_y, col_z};

      p._position = p._position + dir * dt_s * _particle_velocity + _wind_direction * dt_s * _particle_velocity;
      p._z = col_z;
      p._age += dt_s;

      if (p._age > p._lifetime)
      {
         p.spawn(_clip_rect);
      }
   }
}

void Dust::draw(sf::RenderTarget& target, sf::RenderTarget& /*normal*/)
{
   static const auto alpha_default = 50;

   sf::RenderStates states;
   states.blendMode = sf::BlendAlpha;

   std::size_t vertex_index = 0;

   sf::Vertex quad[4];
   for (const auto& p : _particles)
   {
      const auto pos_px = p._position;
      auto alpha = 0.0f;

      if (p._age > p._lifetime - 1.0f)
      {
         alpha = (p._lifetime - p._age) * alpha_default;
      }
      else if (p._age < 1.0f)
      {
         alpha = p._age * alpha_default;
      }
      else
      {
         alpha = alpha_default + p._z * 50.0f;
      }

      const auto color =
         sf::Color{_particle_color.r, _particle_color.g, _particle_color.b, static_cast<uint8_t>(std::clamp(alpha, 0.0f, 255.0f))};

      if (vertex_index + 6 > _vertices.getVertexCount())
      {
         break;
      }

      // triangle 1: top-left, top-right, bottom-right
      _vertices[vertex_index + 0] = sf::Vertex({pos_px.x, pos_px.y}, color);
      _vertices[vertex_index + 1] = sf::Vertex({pos_px.x + _particle_size_px, pos_px.y}, color);
      _vertices[vertex_index + 2] = sf::Vertex({pos_px.x + _particle_size_px, pos_px.y + _particle_size_px}, color);

      // triangle 2: top-left, bottom-right, bottom-left
      _vertices[vertex_index + 3] = sf::Vertex({pos_px.x, pos_px.y}, color);
      _vertices[vertex_index + 4] = sf::Vertex({pos_px.x + _particle_size_px, pos_px.y + _particle_size_px}, color);
      _vertices[vertex_index + 5] = sf::Vertex({pos_px.x, pos_px.y + _particle_size_px}, color);

      vertex_index += 6;

      // old approach where each dust particle was drawn individually
      //
      // quad[0].position = {pos.x, pos.y};                                          // bottom-left
      // quad[1].position = {pos.x, pos.y + _particle_size_px};                      // top-left
      // quad[2].position = {pos.x + _particle_size_px, pos.y};                      // bottom-right
      // quad[3].position = {pos.x + _particle_size_px, pos.y + _particle_size_px};  // top-right
      //
      // for (auto& v : quad)
      // {
      //    v.color = color;
      // }
      //
      // target.draw(quad, 4, sf::PrimitiveType::TriangleStrip, states);
   }

   target.draw(&_vertices[0], vertex_index, sf::PrimitiveType::Triangles, states);
}

std::optional<sf::FloatRect> Dust::getBoundingBoxPx()
{
   return _clip_rect;
}

std::shared_ptr<Dust> Dust::deserialize(GameNode* parent, const GameDeserializeData& data)
{
   auto dust = std::make_shared<Dust>(parent);
   dust->setObjectId(data._tmx_object->_name);

   std::string flowfield_texture = "data/effects/flowfield_3.png";

   const auto clip_rect =
      sf::FloatRect{{data._tmx_object->_x_px, data._tmx_object->_y_px}, {data._tmx_object->_width_px, data._tmx_object->_height_px}};

   dust->_clip_rect = clip_rect;
   dust->addChunks(clip_rect);

   if (data._tmx_object->_properties)
   {
      const auto z_it = data._tmx_object->_properties->_map.find("z");
      const auto particle_size_it = data._tmx_object->_properties->_map.find("particle_size_px");
      const auto particle_count_it = data._tmx_object->_properties->_map.find("particle_count");
      const auto color_it = data._tmx_object->_properties->_map.find("particle_color");
      const auto velocity_it = data._tmx_object->_properties->_map.find("particle_velocity");
      const auto wind_dir_x_it = data._tmx_object->_properties->_map.find("wind_dir_x");
      const auto wind_dir_y_it = data._tmx_object->_properties->_map.find("wind_dir_y");
      const auto flowfield_texture_it = data._tmx_object->_properties->_map.find("flowfield_texture");

      if (z_it != data._tmx_object->_properties->_map.end())
      {
         dust->setZ(z_it->second->_value_int.value());
      }

      if (particle_size_it != data._tmx_object->_properties->_map.end())
      {
         dust->_particle_size_px = static_cast<uint8_t>(particle_size_it->second->_value_int.value());
      }

      if (particle_count_it != data._tmx_object->_properties->_map.end())
      {
         const auto particle_count = particle_count_it->second->_value_int.value();
         for (auto i = 0; i < particle_count; i++)
         {
            Particle p;
            p.spawn(dust->_clip_rect);
            dust->_particles.push_back(p);
         }

         dust->_vertices.resize(6 * particle_count);
      }

      if (wind_dir_x_it != data._tmx_object->_properties->_map.end())
      {
         dust->_wind_direction.x = wind_dir_x_it->second->_value_float.value();
      }

      if (wind_dir_y_it != data._tmx_object->_properties->_map.end())
      {
         dust->_wind_direction.y = wind_dir_y_it->second->_value_float.value();
      }

      if (color_it != data._tmx_object->_properties->_map.end())
      {
         const auto rgba = TmxTools::color(color_it->second->_value_string.value());
         ;
         dust->_particle_color = {rgba[0], rgba[1], rgba[2]};
      }

      if (velocity_it != data._tmx_object->_properties->_map.end())
      {
         dust->_particle_velocity = velocity_it->second->_value_float.value();
      }

      if (flowfield_texture_it != data._tmx_object->_properties->_map.end())
      {
         flowfield_texture = flowfield_texture_it->second->_value_string.value();
      }
   }

   // remember the instance so it's not instantly removed from the cache and each
   // dust instance has to reload the texture
   dust->_flow_field_texture = TexturePool::getInstance().get(flowfield_texture);
   dust->_flow_field_image = dust->_flow_field_texture->copyToImage();

   return dust;
}

void Dust::Particle::spawn(sf::FloatRect& rect)
{
   _position.x = rect.position.x + std::rand() % static_cast<int32_t>(rect.size.x);
   _position.y = rect.position.y + std::rand() % static_cast<int32_t>(rect.size.y);
   _age = 0.0f;
   _lifetime = 5.0f + (std::rand() % 100) * 0.1f;
}
