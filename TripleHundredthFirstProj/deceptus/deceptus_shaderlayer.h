#pragma once

#include "deceptus_gamedeserializedata.h"
#include "deceptus_gamenode.h"
#include "deceptus_gamemechanism.h"

#include <SFML_Graphics.hpp>
#include <filesystem>

struct TmxObject;

struct ShaderLayer : public GameMechanism, public GameNode
{
   ShaderLayer(GameNode* parent = nullptr);
   void draw(sf::RenderTarget& target, sf::RenderTarget& normal) override;
   void update(const sf::Time& dt) override;
   std::optional<sf::FloatRect> getBoundingBoxPx() override;

   sf::Shader _shader;
   sf::Vector2f _position;
   sf::Vector2f _size;
   sf::FloatRect _rect;
   std::shared_ptr<sf::Texture> _texture;
   float _time_offset = 0.0f;
   float _uv_width = 1.0f;
   float _uv_height = 1.0f;
   sf::Time _elapsed;

   static std::shared_ptr<ShaderLayer> deserialize(GameNode* parent, const GameDeserializeData& data);

   // customization factory
   using FactoryFunction = std::shared_ptr<ShaderLayer>(GameNode* parent);
   static std::map<std::string, std::function<FactoryFunction>> __customizations;
   static void registerCustomization(const std::string& id, const std::function<FactoryFunction>&);
};
