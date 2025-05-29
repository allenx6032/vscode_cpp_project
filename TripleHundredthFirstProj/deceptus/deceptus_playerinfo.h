#pragma once

#include <memory>
#include <string>
#include <vector>

#include "json.hpp"

#include "deceptus_extratable.h"
#include "deceptus_inventory.h"
#include "deceptus_playerstats.h"
#include "deceptus_weaponsystem.h"

struct PlayerInfo
{
   Inventory _inventory;
   ExtraTable _extra_table;
   PlayerStats _stats;
   WeaponSystem _weapons;

   std::string _name;
};

void to_json(nlohmann::json& j, const PlayerInfo& d);
void from_json(const nlohmann::json& j, PlayerInfo& d);
