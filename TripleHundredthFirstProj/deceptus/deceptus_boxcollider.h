#pragma once

#include <b2_box2d.h>
#include <memory>

#include "deceptus_gamedeserializedata.h"
#include "deceptus_gamenode.h"
#include "deceptus_gamemechanism.h"

class BoxCollider : public GameMechanism, public GameNode
{
public:
   BoxCollider(GameNode* node);
   void setup(const GameDeserializeData& data);
   std::optional<sf::FloatRect> getBoundingBoxPx() override;

private:
   void setupBody(const std::shared_ptr<b2World>& world);
   b2Body* _body = nullptr;
   sf::Vector2f _size;
   sf::FloatRect _rect;
};
