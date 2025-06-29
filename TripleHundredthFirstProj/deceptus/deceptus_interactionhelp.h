#pragma once

#include <array>
#include <memory>

#include "deceptus_animationpool.h"
#include "deceptus_gamedeserializedata.h"
#include "deceptus_gamenode.h"
#include "deceptus_gamemechanism.h"

struct TmxObject;

class InteractionHelp : public GameMechanism, public GameNode
{
public:
   InteractionHelp(GameNode* parent = nullptr);
   void draw(sf::RenderTarget& target, sf::RenderTarget& normal) override;
   void update(const sf::Time& dt) override;
   void deserialize(const GameDeserializeData& data);
   std::optional<sf::FloatRect> getBoundingBoxPx() override;

private:
   enum class InteractionType
   {
      Read,
      Examine,
      Use
   };

   sf::FloatRect _rect_px;
   bool _player_intersected_in_last_frame{false};
   bool _active{false};

   std::shared_ptr<Animation> _animation_show;
   std::shared_ptr<Animation> _animation_hide;

   std::optional<float> _button_alpha;
   std::unique_ptr<sf::Sprite> _button_sprite;
   std::shared_ptr<sf::Texture> _button_texture;

   sf::Font _font;
   std::unique_ptr<sf::Text> _text;

   sf::IntRect _button_rect_keyboard;
   sf::IntRect _button_rect_controller;
};
