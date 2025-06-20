#include "deceptus_bow.h"

#include "deceptus_arrow.h"
#include "deceptus_globalclock.h"
#include "deceptus_texturepool.h"

#include <array>
#include <iostream>

// http://www.iforce2d.net/b2dtut/sticky-projectiles

// |         |         |         |         |
// |         |         |         |         |
// |         |   ######x######   |         |
// |      #######      |      #######      |
// +-----x---+---------+---------+---x-----+---+ y = 0
// |      #######      |      #######      |
// |         |   ######x######   |         |
// |         |         |         |         |
//                   x = 0

namespace
{

// those two need to be configurable (dynamic objects need higher launch speeds apparently)
constexpr auto launch_speed = 5.0f;
constexpr auto arrow_gravity_scale = 0.1f;

constexpr auto arrow_tail = -1.4f;
constexpr auto arrow_tip = 1.4f;
constexpr auto arrow_width = 0.1f;
constexpr auto scale = 0.1f;

constexpr auto start_frame = 15;
constexpr auto frames_per_row = 15;
constexpr auto animation_frame_count = 4;

uint16_t category_bits = CategoryEnemyCollideWith;                  // I am a ...
uint16_t mask_bits_standing = CategoryBoundary | CategoryFriendly;  // I collide with ...
int16_t group_index = 0;                                            // 0 is default
}  // namespace

WeaponProperties Bow::_default_properties;

Bow::Bow(const WeaponProperties& properties)
{
   _type = WeaponType::Bow;

   const auto use_interval = properties.read<int32_t>("use_interval_ms", 1500);
   setUseIntervalMs(use_interval);
   setLauncherBody(properties._parent_body);

   // the shape is only defined here to align the texture on it
   _shape = std::make_unique<b2PolygonShape>();
   dynamic_cast<b2PolygonShape*>(_shape.get())->SetAsBox((fabs(arrow_tail) + fabs(arrow_tip)) * scale, arrow_width * scale);

   // create reference animation from frame data
   AnimationFrameData frame_data(
      TexturePool::getInstance().get("data/weapons/arrow.png"),
      {static_cast<float_t>(PIXELS_PER_TILE / 2), static_cast<float_t>(PIXELS_PER_TILE / 2)},
      PIXELS_PER_TILE,
      PIXELS_PER_TILE,
      animation_frame_count,
      frames_per_row,
      {sf::seconds(0.075f), sf::seconds(0.075f), sf::seconds(0.075f), sf::seconds(0.075f)},
      start_frame
   );

   setProjectileAnimation(frame_data);
}

void Bow::load(b2World* world)
{
   auto arrow = new Arrow();
   arrow->setAnimation(_projectile_reference_animation._animation);
   arrow->_start_time = GlobalClock::getInstance().getElapsedTimeInMs();

   _loaded_arrow = arrow;

   _loaded_arrow->addDestroyedCallback(
      [this, arrow]()
      {
         // std::cout << "remove " << arrow << " from _projectiles" << std::endl;
         _projectiles.erase(std::remove(_projectiles.begin(), _projectiles.end(), arrow), _projectiles.end());
      }
   );

   b2BodyDef body_def;
   body_def.type = b2_dynamicBody;
   body_def.position.Set(0, 5);

   b2PolygonShape polygon_shape;
   std::array<b2Vec2, 4> vertices;
   vertices[0].Set(arrow_tail * scale, 0.0f);
   vertices[1].Set(0.0, -arrow_width * scale);
   vertices[2].Set(arrow_tip * scale, 0.0f);
   vertices[3].Set(0.0, arrow_width * scale);
   polygon_shape.Set(vertices.data(), 4);

   b2FixtureDef fixture_def;
   fixture_def.shape = &polygon_shape;
   fixture_def.density = 1.0f;
   fixture_def.filter.groupIndex = group_index;
   fixture_def.filter.maskBits = mask_bits_standing;
   fixture_def.filter.categoryBits = category_bits;

   auto loaded_arrow_body = world->CreateBody(&body_def);
   auto fixture = loaded_arrow_body->CreateFixture(&fixture_def);
   loaded_arrow_body->SetAngularDamping(3);
   loaded_arrow_body->SetGravityScale(0.0f);
   fixture->SetUserData(_loaded_arrow);

   _loaded_arrow->setBody(loaded_arrow_body);
}

void Bow::use(const std::shared_ptr<b2World>& world, const b2Vec2& pos, const b2Vec2& dir)
{
   // the bow workflow could be split up into
   // 1) aim
   // 2) pull
   // 3) release
   // Right now it's just firing into walking direction.
   load(world.get());

   // store projectile so it gets drawn
   _projectiles.push_back(_loaded_arrow);

   const auto angle = atan2(dir.y, dir.x);
   const auto velocity = _launcher_body->GetWorldVector(launch_speed * dir);

   _loaded_arrow->getBody()->SetAwake(true);
   _loaded_arrow->getBody()->SetGravityScale(arrow_gravity_scale);
   _loaded_arrow->getBody()->SetAngularVelocity(0.0f);
   _loaded_arrow->getBody()->SetTransform(pos, angle);
   _loaded_arrow->getBody()->SetLinearVelocity(velocity);
   _loaded_arrow->setProperty("damage", _damage);

   // store audio update data if present
   if (_parent_audio_update_data.has_value())
   {
      _loaded_arrow->setParentAudioUpdateData(_parent_audio_update_data.value());
   }

   updateRotation(_loaded_arrow);
   copyReferenceAnimation(_loaded_arrow);

   _loaded_arrow = nullptr;
}

b2Body* Bow::getLauncherBody() const
{
   return _launcher_body;
}

void Bow::setLauncherBody(b2Body* launcher_body)
{
   _launcher_body = launcher_body;
}

void Bow::updateRotation(Arrow* arrow)
{
   constexpr auto minimum_velocity = 0.1f;

   auto* arrow_body = arrow->getBody();
   auto arrow_velocity = arrow_body->GetLinearVelocity();

   if (arrow_velocity.Length() > minimum_velocity)
   {
      arrow_velocity.Normalize();
      arrow->setRotation(atan2(arrow_velocity.y, arrow_velocity.x));
   }
}

void Bow::update(const WeaponUpdateData& data)
{
   Gun::update(data);

   // position the loaded arrow
   if (_loaded_arrow != nullptr)
   {
      // position the arrow next to the object carrying it (half a meter away)
      const auto start_position = _launcher_body->GetWorldPoint(b2Vec2(0.5f, 0));
      _loaded_arrow->getBody()->SetTransform(start_position, _launcher_body->GetAngle());
   }

   // apply drag force to arrows
   for (auto& arrow : _projectiles)
   {
      if (!arrow->getBody()->IsEnabled())
      {
         continue;
      }

      updateRotation(dynamic_cast<Arrow*>(arrow));
   }
}

int32_t Bow::getDamage() const
{
   return 20;
}

std::string Bow::getName() const
{
   return "bow";
}
