#ifndef PLAYERATTACK_H
#define PLAYERATTACK_H

#include <b2_box2d.h>
#include <SFML_Graphics.hpp>
#include <chrono>

class PlayerAnimation;
class PlayerControls;
class WeaponSystem;

struct PlayerAttack
{
   using HighResTimePoint = std::chrono::high_resolution_clock::time_point;

   bool _attack_button_was_pressed = false;
   bool _attack_button_pressed = false;

   HighResTimePoint _timepoint_attack_start;
   HighResTimePoint _timepoint_attack_bend_down_start;
   HighResTimePoint _timepoint_attack_standing_start;
   HighResTimePoint _timepoint_attack_jumping_start;

   enum class AttackResult
   {
      Executed,
      Discarded
   };

   AttackResult attack(
      const std::shared_ptr<b2World>& world,
      const std::shared_ptr<PlayerControls>& controls,
      const std::shared_ptr<PlayerAnimation>& animation,
      const sf::Vector2f& player_pos_px,
      bool points_to_left,
      bool in_air
   );

   bool isAttacking() const;
};

#endif  // PLAYERATTACK_H
