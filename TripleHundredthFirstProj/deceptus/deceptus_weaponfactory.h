#pragma once

#include <b2_box2d.h>

#include "deceptus_weapon.h"
#include "deceptus_weaponproperties.h"

namespace WeaponFactory
{

std::shared_ptr<Weapon> create(WeaponType type);
std::shared_ptr<Weapon> create(WeaponType type, const WeaponProperties& properties);
}  // namespace WeaponFactory
