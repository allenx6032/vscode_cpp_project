#pragma once

#include <cstdint>

#include "deceptus_projectile.h"

struct Arrow : public Projectile
{
   Arrow();
   ~Arrow();
   int32_t _start_time = 0;
   static bool _animation_initialised;
};
