#pragma once

#include <SFML_Graphics.hpp>
#include <memory>

class AtmosphereShader
{
public:
   AtmosphereShader(uint32_t texture_width, uint32_t texture_height);
   ~AtmosphereShader();

   void initialize();
   void update();

   const std::shared_ptr<sf::RenderTexture>& getRenderTexture() const;
   const sf::Shader& getShader() const;

private:
   std::shared_ptr<sf::RenderTexture> _render_texture;
   std::shared_ptr<sf::Texture> _distortion_map;
   sf::Shader _shader;
};
