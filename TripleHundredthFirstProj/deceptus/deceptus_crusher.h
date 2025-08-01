#pragma once

#include "deceptus_constants.h"
#include "deceptus_gamedeserializedata.h"
#include "deceptus_gamenode.h"
#include "deceptus_gamemechanism.h"

#include <b2_box2d.h>

struct TmxObject;

class Crusher : public GameMechanism, public GameNode
{
public:
   enum class Mode
   {
      Interval,
      Distance
   };

   enum class State
   {
      Idle,
      Extract,
      Retract
   };

   Crusher(GameNode* parent = nullptr);

   void draw(sf::RenderTarget& color, sf::RenderTarget& normal) override;
   void update(const sf::Time& dt) override;
   std::optional<sf::FloatRect> getBoundingBoxPx() override;

   void setup(const GameDeserializeData& data);

private:
   void updateTransform();
   void setupBody(const std::shared_ptr<b2World>& world);

   void step(const sf::Time& dt);
   void updateState();
   void updateSpritePositions();
   void startBoomEffect();
   void stopBoomEffect();

   Mode _mode = Mode::Interval;
   State _state = State::Idle;
   State _state_previous = State::Idle;
   Alignment _alignment = Alignment::PointsDown;

   b2Body* _body{nullptr};
   sf::Vector2f _pixel_position;
   sf::Vector2f _blade_offset;
   sf::FloatRect _rect;

   sf::Time _idle_time;
   sf::Time _extraction_time;
   sf::Time _retraction_time;
   sf::Time _idle_time_max;
   sf::Time _extraction_time_max;
   sf::Time _retraction_time_max;
   sf::Time _time_offset;

   std::unique_ptr<sf::Sprite> _sprite_spike;
   std::unique_ptr<sf::Sprite> _sprite_pusher;
   std::unique_ptr<sf::Sprite> _sprite_mount;
   sf::Vector2f _pixel_offset_mount;
   sf::Vector2f _pixel_offset_pusher;
   sf::Vector2f _pixel_offset_spike;

   bool _shake{true};
   bool _shake_shown{false};

   int32_t _instance_id{0};

   std::shared_ptr<sf::Texture> _texture;
   static int32_t __instance_counter;
};
