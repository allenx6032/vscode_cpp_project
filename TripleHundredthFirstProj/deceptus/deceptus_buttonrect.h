#pragma once

#include "deceptus_gamedeserializedata.h"
#include "deceptus_gamenode.h"
#include "deceptus_gamemechanism.h"

class ButtonRect : public GameMechanism, public GameNode
{
public:
   ButtonRect(GameNode* parent = nullptr);

   enum class Action
   {
      Enable,
      Disable,
      Toggle,
   };

   enum class Button
   {
      A,
      B,
      X,
      Y
   };

   void update(const sf::Time& dt) override;
   std::optional<sf::FloatRect> getBoundingBoxPx() override;

   void setup(const GameDeserializeData& data);
   void findReference(const std::vector<std::shared_ptr<GameMechanism>>& mechanisms);


private:
   void processAction();

   sf::FloatRect _rect;
   bool _player_intersects = false;
   Button _botton{Button::B};
};
