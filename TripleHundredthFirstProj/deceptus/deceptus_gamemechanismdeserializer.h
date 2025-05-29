#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include "deceptus_tmxparser.h"
#include "deceptus_gamedeserializedata.h"
#include "deceptus_gamenode.h"
#include "deceptus_gamemechanism.h"

namespace GameMechanismDeserializer
{

void deserialize(
   const TmxParser& tmx_parser,
   GameNode* parent,
   const GameDeserializeData& data,
   std::unordered_map<std::string, std::vector<std::shared_ptr<GameMechanism>>*>& mechanisms
);

bool isLayerNameReserved(const std::string& layer_name);

}  // namespace GameMechanismDeserializer
