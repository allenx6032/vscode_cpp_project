#pragma once

#include "deceptus_gamedeserializedata.h"
#include "deceptus_gamenode.h"
#include "deceptus_gamemechanism.h"

#include <functional>
#include "SFML_Graphics.hpp"

class SensorRect : public GameMechanism, public GameNode
{
public:
   SensorRect(GameNode* parent = nullptr);

   enum class Action
   {
      Enable,
      Disable,
      Toggle,
   };

   enum class Event
   {
      OnEnter,
      OnLeave,
   };

   void update(const sf::Time& dt) override;
   std::optional<sf::FloatRect> getBoundingBoxPx() override;

   void setup(const GameDeserializeData& data);
   void findReference(const std::vector<std::shared_ptr<GameMechanism>>& mechanisms);

   using SensorCallback = std::function<void(const std::string& id)>;
   void addSensorCallback(const SensorCallback& callback);

private:
   void processAction();

   sf::FloatRect _rect;
   bool _player_intersects = false;
   Event _event = Event::OnEnter;
   Action _action = Action::Enable;
   std::string _reference_id;
   std::vector<std::shared_ptr<GameMechanism>> _references;
   std::vector<SensorCallback> _callbacks;
};
