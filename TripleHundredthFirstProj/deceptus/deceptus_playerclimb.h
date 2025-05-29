#pragma once

#include <b2_box2d.h>
#include <functional>
#include "deceptus_playercontrols.h"

class b2Joint;
class b2ChainShape;

struct PlayerClimb
{
   PlayerClimb() = default;

   void update(b2Body* body, bool in_air);
   void removeClimbJoint();
   bool isClimbableEdge(b2ChainShape* shape, int currIndex);
   bool edgeMatchesMovement(const b2Vec2& edgeDir);
   bool isClimbing() const;

   b2Joint* _climb_joint = nullptr;
   int32_t _keys_pressed = 0;

   std::shared_ptr<PlayerControls> _controls;

public:
   void setControls(const std::shared_ptr<PlayerControls>& newControls);
};
