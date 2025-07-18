#include "deceptus_camerapanorama.h"

#include "deceptus_gamecontroller.h"
#include "deceptus_sfmlmath.h"
#include "deceptus_cameraroomlock.h"
#include "deceptus_tweaks.h"
#include "deceptus_gamecontrollerdata.h"
#include "deceptus_gamecontrollerintegration.h"
#include "deceptus_displaymode.h"

CameraPanorama& CameraPanorama::getInstance()
{
   static CameraPanorama __instance;
   return __instance;
}

sf::Vector2f CameraPanorama::computeDesiredLookVector(
   bool looking_up,
   bool looking_down,
   bool looking_left,
   bool looking_right,
   bool can_look_up,
   bool can_look_down,
   bool can_look_left,
   bool can_look_right,
   float x_relative = 1.0f,
   float y_relative = 1.0f
) const
{
   const auto& tweaks = Tweaks::instance();

   auto limit_look_vector = [&](sf::Vector2f& look_vector)
   {
      if (DisplayMode::getInstance().isSet(Display::IngameMenu))
      {
         return;
      }
      const auto len = SfmlMath::length(look_vector);
      if (len > tweaks._cpan_max_distance_px)
      {
         look_vector = SfmlMath::normalize(look_vector);
         look_vector *= tweaks._cpan_max_distance_px;
      }
   };

   sf::Vector2f desired_look_vector = _look_vector;

   if (looking_up && can_look_up)
   {
      desired_look_vector += sf::Vector2f(0.0f, -tweaks._cpan_look_speed_y * y_relative);
   }
   else if (looking_down && can_look_down)
   {
      desired_look_vector += sf::Vector2f(0.0f, tweaks._cpan_look_speed_y * y_relative);
   }

   if (looking_left && can_look_left)
   {
      desired_look_vector += sf::Vector2f(-tweaks._cpan_look_speed_x * x_relative, 0.0f);
   }
   else if (looking_right && can_look_right)
   {
      desired_look_vector += sf::Vector2f(tweaks._cpan_look_speed_x * x_relative, 0.0f);
   }

   if (!tweaks._cpan_unlimited)
   {
      limit_look_vector(desired_look_vector);
   }

   return desired_look_vector;
}

void CameraPanorama::update()
{
   const auto& tweaks = Tweaks::instance();
   const auto result = CameraRoomLock::checkRoomBoundaries();

   bool changed = false;

   const auto locked_up = result[0];
   const auto locked_down = result[1];
   const auto locked_left = result[2];
   const auto locked_right = result[3];

   if (_look_state & static_cast<int32_t>(Look::Active))
   {
      // only update the desired look vector when boundaries are not exceeded
      const auto looking_up = _look_state & static_cast<int32_t>(Look::Up);
      const auto looking_down = _look_state & static_cast<int32_t>(Look::Down);
      const auto looking_left = _look_state & static_cast<int32_t>(Look::Left);
      const auto looking_right = _look_state & static_cast<int32_t>(Look::Right);

      const auto can_look_up = !(locked_up && _look_vector.y < 0.0f) || tweaks._cpan_unlimited;
      const auto can_look_down = !(locked_down && _look_vector.y > 0.0f) || tweaks._cpan_unlimited;
      const auto can_look_left = !(locked_left && _look_vector.x < 0.0f) || tweaks._cpan_unlimited;
      const auto can_look_right = !(locked_right && _look_vector.x > 0.0f) || tweaks._cpan_unlimited;

      // clang-format off
      const auto desired_look_vector = computeDesiredLookVector(
         looking_up,
         looking_down,
         looking_left,
         looking_right,
         can_look_up,
         can_look_down,
         can_look_left,
         can_look_right
      );
      // clang-format on

      updateLookVector(desired_look_vector);
      changed = true;
   }
   else if (GameControllerIntegration::getInstance().isControllerConnected())
   {
      const auto& axis_values = GameControllerData::getInstance().getJoystickInfo().getAxisValues();
      const auto x_axis = GameControllerIntegration::getInstance().getController()->getAxisIndex(SDL_GAMEPAD_AXIS_RIGHTX);
      const auto y_axis = GameControllerIntegration::getInstance().getController()->getAxisIndex(SDL_GAMEPAD_AXIS_RIGHTY);
      const auto x_normalized = axis_values[static_cast<uint32_t>(x_axis)] / 32767.0f;
      const auto y_normalized = axis_values[static_cast<uint32_t>(y_axis)] / 32767.0f;
      const auto tolerance_x = tweaks._cpan_tolerance_x;
      const auto tolerance_y = tweaks._cpan_tolerance_y;

      if (fabs(x_normalized) > tolerance_x || fabs(y_normalized) > tolerance_y)
      {
         // compute values from 0..1 removing the tolerance gap at the beginning
         const auto x_relative = (fabs(x_normalized) - tolerance_x) / (1.0f - tolerance_x);
         const auto y_relative = (fabs(y_normalized) - tolerance_y) / (1.0f - tolerance_y);

         const auto looking_left = x_normalized < 0.0f;
         const auto looking_right = x_normalized > 0.0f;
         const auto looking_up = y_normalized < 0.0f;
         const auto looking_down = y_normalized > 0.0f;

         const auto can_look_left = !(locked_left && _look_vector.x < 0.0f);
         const auto can_look_right = !(locked_right && _look_vector.x > 0.0f);
         const auto can_look_up = !(locked_up && _look_vector.y < 0.0f);
         const auto can_look_down = !(locked_down && _look_vector.y > 0.0f);

         // clang-format off
         const auto desired_look_vector = computeDesiredLookVector(
            looking_up,
            looking_down,
            looking_left,
            looking_right,
            can_look_up,
            can_look_down,
            can_look_left,
            can_look_right,
            x_relative,
            y_relative
         );
         // clang-format on

         updateLookVector(desired_look_vector);
         changed = true;

         if (!DisplayMode::getInstance().isSet(Display::CameraPanorama))
         {
            DisplayMode::getInstance().enqueueSet(Display::CameraPanorama);
         }
      }
      else
      {
         if (DisplayMode::getInstance().isSet(Display::CameraPanorama))
         {
            DisplayMode::getInstance().enqueueUnset(Display::CameraPanorama);
         }
      }
   }

   if (!changed)
   {
      _look_vector *= tweaks._cpan_snap_back_factor;
   }
}

void CameraPanorama::processKeyPressedEvents(const sf::Event::KeyPressed* key_event)
{
   switch (key_event->code)
   {
      case sf::Keyboard::Key::LShift:
      {
         updateLookState(Look::Active, true);
         DisplayMode::getInstance().enqueueSet(Display::CameraPanorama);
         break;
      }
      case sf::Keyboard::Key::Left:
      {
         updateLookState(Look::Left, true);
         break;
      }
      case sf::Keyboard::Key::Right:
      {
         updateLookState(Look::Right, true);
         break;
      }
      case sf::Keyboard::Key::Up:
      {
         updateLookState(Look::Up, true);
         break;
      }
      case sf::Keyboard::Key::Down:
      {
         updateLookState(Look::Down, true);
         break;
      }
      default:
      {
         break;
      }
   }
}

void CameraPanorama::processKeyReleasedEvents(const sf::Event::KeyReleased* key_event)
{
   switch (key_event->code)
   {
      case sf::Keyboard::Key::LShift:
      {
         updateLookState(Look::Active, false);
         DisplayMode::getInstance().enqueueUnset(Display::CameraPanorama);
         break;
      }
      case sf::Keyboard::Key::Left:
      {
         updateLookState(Look::Left, false);
         break;
      }
      case sf::Keyboard::Key::Right:
      {
         updateLookState(Look::Right, false);
         break;
      }
      case sf::Keyboard::Key::Up:
      {
         updateLookState(Look::Up, false);
         break;
      }
      case sf::Keyboard::Key::Down:
      {
         updateLookState(Look::Down, false);
         break;
      }
      default:
      {
         break;
      }
   }
}

void CameraPanorama::updateLookState(Look look, bool enable)
{
   if (enable)
   {
      _look_state |= static_cast<int32_t>(look);
   }
   else
   {
      _look_state &= ~static_cast<int32_t>(look);
   }
}

void CameraPanorama::updateLookVector(const sf::Vector2f& desired)
{
   _look_vector = desired;
}

bool CameraPanorama::isKeyboardLookActive() const
{
   return (_look_state & static_cast<int32_t>(Look::Active));
}

const sf::Vector2f& CameraPanorama::getLookVector() const
{
   return _look_vector;
}

// so far not resolved issue, could be considered if needed:
//
// the player is allowed to move 'freely' within the camera system's focus zone (f0..f1).
// that's usually a horizontal range within the center of the screen. that range can be
// shifted around a bit depending on the player movement (if enabled in the camera system).
// however, when the player is more on the left or right side of the focus zone, then the
// range of the camera panorama should be adjusted.
//
//
// default use case A
// +-------------+-------------+
// |       :     |     :       |
// |       :     |     :       |
// |       :     |     :       |
// |       :     |     :       |
// +-------:-----p-----:-------+
// |       :     |     :       |
// |       :     |     :       |
// +-------------+-------------+
//         f0          f1
//
// outside of center use case B                        outside of center use case C
// +-----------------+---------+                       +---------+-----------------+
// |       :         | :       |                       |       : |         :       |
// |       :         | :       |                       |       : |         :       |
// |       :         | :       |                       |       : |         :       |
// |       :         | :       |                       |       : |         :       |
// +-------:-----|---p-:-------+                       +-------:-p---|-----:-------+
// |       :         | :       |                       |       : |         :       |
// |       :         | :       |                       |       : |         :       |
// +-----------------+---------+                       +---------+-----------------+
//         f0          f1                                      f0          f1
//
//                    player is on the right side              player is on the left side
//                    of the focus zone                        of the focus zone
//                    camera system dx = 90px                  camera system dx = -90
//
//
// in use case B, already a large portion of the left-facing side of the screen is visible (90px).
// by using the camera panorama these 90px should be subtracted from the cpan range when the user
// wants to navigate the cpan to the left.
//
// player is standing in the center of the focus zone (A)
//    dx = 0
//    -> cpan should go from -100; 100
//
// player standing on the right of the focus zone (B)
//    dx = 90
//    -> cpan should go from -10 .. 190
//
// player standing on the left of the focus zone (C)
//    dx = -90
//    -> cpan should go from -10
//
//    -> should go from -100 to 100
//
// std::cout << CameraSystem::getInstance().getDx() << " " << CameraSystem::getInstance().getDy() << std::endl;
