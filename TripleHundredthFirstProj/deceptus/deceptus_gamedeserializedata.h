#pragma once

#include <b2_box2d.h>
#include <filesystem>
#include "deceptus_tmxlayer.h"
#include "deceptus_tmxobject.h"
#include "deceptus_tmxobjectgroup.h"
#include "deceptus_tmxtileset.h"

struct GameDeserializeData
{
   std::filesystem::path _base_path;
   std::shared_ptr<b2World> _world;
   std::shared_ptr<TmxObject> _tmx_object;
   std::shared_ptr<TmxObjectGroup> _tmx_object_group;
   std::shared_ptr<TmxLayer> _tmx_layer;
   std::shared_ptr<TmxTileSet> _tmx_tileset;
};
