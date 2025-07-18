#include "deceptus_playercontrolstate.h"

#include "deceptus_displaymode.h"
#include "deceptus_gamestate.h"

bool PlayerControlState::checkState()
{
   const auto& display_mode = DisplayMode::getInstance();
   if (display_mode.isSet(Display::Modal) || display_mode.isSet(Display::CameraPanorama))
   {
      return false;
   }

   if (GameState::getInstance().getMode() != ExecutionMode::Running)
   {
      return false;
   }

   return true;
}

bool PlayerControlState::checkStateCpanOkay()
{
   const auto& display_mode = DisplayMode::getInstance();
   if (display_mode.isSet(Display::Modal))
   {
      return false;
   }

   if (GameState::getInstance().getMode() != ExecutionMode::Running)
   {
      return false;
   }

   return true;
}

bool PlayerControlState::checkStateUseInventory()
{
   const auto& display_mode = DisplayMode::getInstance();
   if (display_mode.isSet(Display::Modal) || display_mode.isSet(Display::IngameMenu))
   {
      return false;
   }

   if (GameState::getInstance().getMode() != ExecutionMode::Running)
   {
      return false;
   }

   return true;
}
