#pragma once

#include "deceptus_room.h"

#include <SFML_Graphics.hpp>

#include <memory>
#include <optional>

class CameraSystem
{
public:
   void update(const sf::Time& dt, float view_width_px, float view_height_px);

   float getX() const;
   float getY() const;

   float getFocusZoneX0() const;
   float getFocusZoneX1() const;

   float getPanicLineY0() const;
   float getPanicLineY1() const;

   float getFocusOffset() const;
   float getFocusZoneCenter() const;

   float getDx() const;
   float getDy() const;

   void syncNow();

   static CameraSystem& getInstance();

private:
   CameraSystem() = default;

   void updateX(const sf::Time& delta_time);
   void updateY(const sf::Time& delta_time);

   float _x_px = 0.0f;
   float _y_px = 0.0f;

   float _dx_px = 0.0f;
   float _dy_px = 0.0f;

   float _focus_zone_x0_px = 0.0f;
   float _focus_zone_x1_px = 0.0f;
   float _focus_zone_center_px = 0.0f;
   float _focus_offset_px = 0.0f;

   float _panic_line_y0_px = 0.0f;
   float _panic_line_y1_px = 0.0f;
   bool _panic = false;

   float _view_width_px = 0.0f;
   float _view_height_px = 0.0f;

   bool _focus_x_triggered = false;
   bool _focus_y_triggered = false;

   bool _no_y_update_triggered = false;
   sf::Time _y_update_start_time;
};
