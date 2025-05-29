#ifndef TREASURECHEST_H
#define TREASURECHEST_H

#include "deceptus_animation.h"
#include "deceptus_spawneffect.h"
#include "deceptus_gamedeserializedata.h"
#include "deceptus_gamenode.h"
#include "deceptus_gamemechanism.h"

class TreasureChest : public GameMechanism, public GameNode
{
public:
   TreasureChest(GameNode* parent = nullptr);

   void deserialize(const GameDeserializeData& data);

   void draw(sf::RenderTarget& target, sf::RenderTarget& /*normal*/) override;
   void update(const sf::Time& dt) override;
   std::optional<sf::FloatRect> getBoundingBoxPx() override;

private:
   enum class Alignment
   {
      Left,
      Right
   };

   enum class State
   {
      Closed,
      Opening,
      Open
   };

   sf::FloatRect _rect;
   Alignment _alignment{Alignment::Left};
   std::shared_ptr<sf::Texture> _texture;
   std::unique_ptr<sf::Sprite> _sprite;
   std::string _sample_open;
   State _state{State::Closed};
   std::optional<std::string> _spawn_extra;

   std::shared_ptr<Animation> _animation_idle_closed;
   std::shared_ptr<Animation> _animation_opening;
   std::shared_ptr<Animation> _animation_idle_open;
   std::unique_ptr<SpawnEffect> _spawn_effect;
};

#endif  // TREASURECHEST_H
