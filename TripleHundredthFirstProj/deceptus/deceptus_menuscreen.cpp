#include "deceptus_menuscreen.h"

#include "deceptus_psd.h"
#include "deceptus_log.h"
#include "deceptus_gamecontrollerintegration.h"

#include <iostream>

void MenuScreen::update(const sf::Time& /*dt*/)
{
}

void MenuScreen::draw(sf::RenderTarget& window, sf::RenderStates states)
{
   for (auto& layer : _layer_stack)
   {
      if (layer->_visible)
      {
         layer->draw(window, states);
      }
   }
}

void MenuScreen::showEvent()
{
}

void MenuScreen::hideEvent()
{
}

const std::string& MenuScreen::getFilename()
{
   return _filename;
}

void MenuScreen::setFilename(const std::string& filename)
{
   _filename = filename;
}

void MenuScreen::load()
{
   PSD psd;
   psd.setColorFormat(PSD::ColorFormat::ABGR);
   psd.load(_filename);

   for (const auto& layer : psd.getLayers())
   {
      // skip groups
      if (!layer.isImageLayer())
      {
         continue;
      }

      auto tmp = std::make_shared<Layer>();

      try
      {
         const auto texture_size = sf::Vector2u(static_cast<uint32_t>(layer.getWidth()), static_cast<uint32_t>(layer.getHeight()));
         auto texture = std::make_shared<sf::Texture>(texture_size);
         auto opacity = layer.getOpacity();

         texture->update(reinterpret_cast<const uint8_t*>(layer.getImage().getData().data()));
         auto sprite = std::make_shared<sf::Sprite>(*texture);

         sprite->setPosition({static_cast<float>(layer.getLeft()), static_cast<float>(layer.getTop())});
         sprite->setColor(sf::Color(255u, 255u, 255u, static_cast<uint8_t>(opacity)));

         tmp->_texture = texture;
         tmp->_sprite = sprite;

         _layer_stack.push_back(tmp);
         _layers[layer.getName()] = tmp;
      }
      catch (...)
      {
         Log::Fatal() << "failed to create texture: " << layer.getName();
      }
   }

   loadingFinished();
}

void MenuScreen::loadingFinished()
{
}

void MenuScreen::keyboardKeyPressed(sf::Keyboard::Key /*key*/)
{
}

void MenuScreen::keyboardKeyReleased(sf::Keyboard::Key /*key*/)
{
}

void MenuScreen::controllerButtonX()
{
   // that's default behavior for most screens
   keyboardKeyPressed(sf::Keyboard::Key::D);
}

void MenuScreen::controllerButtonY()
{
}

bool MenuScreen::isControllerUsed() const
{
   return GameControllerIntegration::getInstance().isControllerConnected();
}
