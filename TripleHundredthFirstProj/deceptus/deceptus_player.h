#pragma once

#include "deceptus_animationpool.h"
#include "deceptus_constants.h"
#include "deceptus_waterbubbles.h"
#include "deceptus_chunk.h"
#include "deceptus_gamenode.h"
#include "deceptus_playeranimation.h"
#include "deceptus_playerattack.h"
#include "deceptus_playerattackdash.h"
#include "deceptus_playerbelt.h"
#include "deceptus_playerbend.h"
#include "deceptus_playerclimb.h"
#include "deceptus_playercontrols.h"
#include "deceptus_playerdash.h"
#include "deceptus_playerjump.h"
#include "deceptus_playerjumptrace.h"
#include "deceptus_playerplatform.h"
#include "deceptus_playerspeed.h"

#include <b2_box2d.h>
#include <SFML_Graphics.hpp>
#include <SFML_System.hpp>

#include <chrono>
#include <deque>
#include <functional>
#include <memory>

class Animation;
class GameContactListener;
struct ScreenTransition;
class Weapon;
struct WeaponSystem;

class Player : public GameNode
{
   using HighResTimePoint = std::chrono::high_resolution_clock::time_point;
   using ToggleCallback = std::function<void()>;

   struct PositionedAnimation
   {
      sf::Vector2f _position;
      std::shared_ptr<Animation> _animation;
   };

public:
   Player(GameNode* parent = nullptr);
   virtual ~Player() = default;

   static Player* getCurrent();

   void initialize();
   void initializeLevel();
   void initializeController();
   void draw(sf::RenderTarget& color, sf::RenderTarget& normal);
   void drawStencil(sf::RenderTarget& color);

   void update(const sf::Time& dt);
   void reloadAnimationPool();

   void die();
   void reset();
   DeathReason checkDead() const;

   bool isPointingRight() const;
   bool isPointingLeft() const;

   void setStartPixelPosition(float x, float y);

   const sf::Vector2f& getPixelPositionFloat() const;
   const sf::Vector2i& getPixelPositionInt() const;
   void setPixelPosition(float x, float y);

   const sf::FloatRect& getPixelRectFloat() const;
   const sf::IntRect& getPixelRectInt() const;

   b2Body* getBody() const;
   b2Fixture* getFootSensorFixture() const;
   sf::IntRect computeFootSensorPixelIntRect() const;
   sf::FloatRect computeFootSensorPixelFloatRect() const;

   void setWorld(const std::shared_ptr<b2World>& world);
   void resetWorld();
   void updatePixelPosition();
   void updatePreviousBodyState();
   void updatePixelRect();
   void updateChunk();
   void setBodyViaPixelPosition(float x, float y);
   void setFriction(float f);
   float getVelocityNormalized() const;

   bool getVisible() const;
   void setVisible(bool visible);
   void fadeOut(float fade_out_speed_factor = 5.0f);
   void fadeOutReset();

   void setGroundBody(b2Body* body);

   bool isInAir() const;
   bool isInWater() const;
   bool isOnGround() const;
   bool isDead() const;
   bool isJumpingThroughOneWayWall();

   void setInWater(bool inWater);

   int getZIndex() const;
   void setZIndex(int32_t z);

   int getId() const;

   void impulse(float intensity);
   void damage(int32_t damage, const sf::Vector2f& force = sf::Vector2f{0.0f, 0.0f});
   void kill(std::optional<DeathReason> death_reason = std::nullopt);
   void goToPortal(auto portal);

   const std::shared_ptr<PlayerControls>& getControls() const;
   const PlayerJump& getJump() const;
   const PlayerBend& getBend() const;
   PlayerBelt& getBelt();
   PlayerPlatform& getPlatform();
   const Chunk& getChunk() const;

   void setToggleCallback(const ToggleCallback& callback);

private:
   void createPlayerBody();

   void updateFadeOut(const sf::Time& dt);
   void updateAnimation(const sf::Time& dt);
   void updateAtmosphere();
   void updateBendDown();
   void updateDash(Dash dir = Dash::None);
   void updateAttack();
   void updateAttackDash(const sf::Time& dt);
   void updateFootsteps();
   void updateGroundAngle();
   void updateHardLanding();
   void updateImpulse();
   void updateOneWayWallDrop();
   void updateChainShapeCollisions();
   void updatePixelCollisions();
   void updateOrientation();
   void updatePortal();
   void updateVelocity();
   void updateWeapons(const sf::Time& dt);
   void updateJump();
   void updateWallslide(const sf::Time& dt);
   void updateWaterBubbles(const sf::Time& dt);
   void updateSpawn();
   void updateHealth(const sf::Time& dt);

   void startHardLanding();
   void resetMotionBlur();

   void createBody();
   void createFeet();
   void setMaskBitsCrouching(bool enabled);

   float getMaxVelocity() const;
   float readVelocityFromController(const PlayerSpeed& speed) const;
   float readVelocityFromKeyboard(const PlayerSpeed& speed) const;
   float readDesiredVelocity(const PlayerSpeed& speed) const;
   float readDesiredVelocity() const;
   float getDeceleration() const;
   float getAcceleration() const;

   void traceJumpCurve();
   void keyPressed(sf::Keyboard::Key key);
   std::unique_ptr<ScreenTransition> makeFadeTransition();

   void drawDash(sf::RenderTarget& color, const std::shared_ptr<Animation>& current_cycle, const sf::Vector2f& draw_position_px);
   bool checkDamageDrawSkip() const;
   void updateHurtColor(const std::shared_ptr<Animation>& current_cycle);

   void useInventory(int32_t slot);

   // all related to player physics and box2d
   std::shared_ptr<b2World> _world;
   b2Body* _body = nullptr;
   static constexpr int32_t __foot_count = 4u;
   b2Fixture* _body_fixture = nullptr;
   b2Fixture* _foot_fixture[__foot_count]{nullptr, nullptr, nullptr, nullptr};
   b2Fixture* _foot_sensor_fixture = nullptr;
   b2Body* _ground_body = nullptr;
   b2Vec2 _ground_normal;
   b2Vec2 _position_previous;
   b2Vec2 _velocity_previous;
   float _impulse{0.0f};

   sf::Vector2f _pixel_position_f;
   sf::Vector2i _pixel_position_i;
   sf::FloatRect _pixel_rect_f;
   sf::IntRect _pixel_rect_i;

   sf::Time _time;
   sf::Clock _clock;
   sf::Clock _portal_clock;
   sf::Clock _damage_clock;
   bool _damage_initialized{false};

   bool _points_to_left{false};
   bool _visible{true};
   bool _fade_out{false};
   float _fade_out_alpha{1.0f};
   float _fade_out_speed_factor{10.0f};
   bool _dead{false};
   bool _spawn_complete{false};
   bool _spawn_orientation_locked{false};
   std::optional<DeathReason> _death_reason;

   bool _in_water{false};
   HighResTimePoint _water_entered_time;

   float _next_footstep_time{0.0f};
   int32_t _step_counter{0};

   int32_t _z_index{0};
   int32_t _id{0};

   bool _hard_landing{false};
   int32_t _hard_landing_cycles{0};
   HighResTimePoint _timepoint_hard_landing;

   std::shared_ptr<PlayerControls> _controls;
   PlayerAttack _attack;
   PlayerAttackDash _attack_dash;
   PlayerBelt _belt;
   PlayerBend _bend;
   PlayerClimb _climb;
   PlayerDash _dash;
   PlayerJump _jump;
   PlayerJumpTrace _jump_trace;
   PlayerPlatform _platform;
   WaterBubbles _water_bubbles;

   std::shared_ptr<PlayerAnimation> _player_animation;
   std::deque<PositionedAnimation> _last_animations;

   Chunk _chunk{0, 0};
   AnimationPool _animation_pool{"data/sprites/animations.json"};

   static Player* __current;
};
