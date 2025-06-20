#include "deceptus_level.h"

// game
#include "deceptus_maptools.h"
#include "deceptus_tmxelement.h"
#include "deceptus_tmxlayer.h"
#include "deceptus_tmxobjectgroup.h"
#include "deceptus_tmxparser.h"
#include "deceptus_tmxtileset.h"
#include "deceptus_checksum.h"
#include "deceptus_log.h"
#include "deceptus_timer.h"
#include "deceptus_animationplayer.h"
#include "deceptus_camerapanorama.h"
#include "deceptus_cameraroomlock.h"
#include "deceptus_camerasystem.h"
#include "deceptus_camerazoom.h"
#include "deceptus_gameconfiguration.h"
#include "deceptus_tweaks.h"
#include "deceptus_constants.h"
#include "deceptus_debugdraw.h"
#include "deceptus_ingamemenumap.h"
#include "deceptus_gamedeserializedata.h"
#include "deceptus_meshtools.h"
#include "deceptus_fixturenode.h"
#include "deceptus_leveldescription.h"
#include "deceptus_levelfiles.h"
#include "deceptus_luainterface.h"
#include "deceptus_parsedata.h"
#include "deceptus_roomupdater.h"
#include "deceptus_tilemap.h"
#include "deceptus_tilemapfactory.h"
#include "deceptus_checkpoint.h"
#include "deceptus_conveyorbelt.h"
#include "deceptus_door.h"
#include "deceptus_extra.h"
#include "deceptus_gamemechanismdeserializer.h"
#include "deceptus_gamemechanismdeserializerconstants.h"
#include "deceptus_lever.h"
#include "deceptus_chainshapeanalyzer.h"
#include "deceptus_gamecontactlistener.h"
#include "deceptus_physicsconfiguration.h"
#include "deceptus_squaremarcher.h"
#include "deceptus_player.h"
#include "deceptus_playerstencil.h"
#include "deceptus_displaymode.h"
#include "deceptus_savestate.h"
#include "deceptus_gun.h"

// sfml
#include <SFML_RenderWindow.hpp>
#include <SFML_Event.hpp>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <thread>

// #define MECHANISM_TIMING_ENABLED 1

#ifdef MECHANISM_TIMING_ENABLED
namespace
{
struct MechanismTiming
{
   using HighResDuration = std::chrono::high_resolution_clock::duration;

   HighResDuration update_duration;
   int32_t update_count{0};

   void addUpdateTime(HighResDuration duration)
   {
      update_duration += duration;
      update_count++;
   }

   auto getAverageUpdateTime() const
   {
      return (update_count > 0) ? (update_duration / update_count) : HighResDuration{0};
   }
};

std::unordered_map<std::string, MechanismTiming> timing_data;

void logUpdateTimes(const std::unordered_map<std::string, MechanismTiming>& timing_data, int32_t n)
{
   std::vector<std::pair<std::string, MechanismTiming::HighResDuration>> timings;
   for (const auto& [name, timing] : timing_data)
   {
      timings.emplace_back(name, timing.getAverageUpdateTime());
   }

   std::ranges::sort(timings, [](const auto& a, const auto& b) { return a.second > b.second; });
   const auto top_n = timings | std::views::take(n);

   Log::Info() << "update times";

   for (const auto& [name, avg_update_time] : top_n)
   {
      Log::Info() << "- mechanism: " << name << " avg: " << avg_update_time << " sum:" << timing_data.at(name).update_duration
                  << " count:" << timing_data.at(name).update_count;
   }
}

}  // namespace
#endif

namespace
{

bool checkUpdateMechanism(const auto& player_chunk, const auto& mechanism)
{
   auto update_mechanism = true;
   if (mechanism->hasChunks())
   {
      const auto& chunks = mechanism->getChunks();
      update_mechanism = std::any_of(
         chunks.cbegin(),
         chunks.cend(),
         [player_chunk](const Chunk& other)
         { return (abs(player_chunk._x - other._x) < CHUNK_ALLOWED_DELTA_X) && (abs(player_chunk._y - other._y) < CHUNK_ALLOWED_DELTA_Y); }
      );
   }

   return update_mechanism;
};

}  // namespace

Level* Level::__current_level = nullptr;

std::string Level::getDescriptionFilename() const
{
   return _description_filename;
}

void Level::setDescriptionFilename(const std::string& description_filename)
{
   _description_filename = description_filename;
}

const Atmosphere& Level::getAtmosphere() const
{
   return _atmosphere;
}

void Level::initializeTextures()
{
   const auto& game_config = GameConfiguration::getInstance();

   // since stencil buffers are used, it is required to enable them explicitly
   sf::ContextSettings stencil_context_settings;
   stencil_context_settings.stencilBits = 8;
   // stencil_context_settings.antialiasingLevel = 8; // makes texture tearing much more reproducable

   _render_texture_level_background.reset();

   _atmosphere_shader.reset();
   _gamma_shader.reset();
   _blur_shader.reset();

   // this the render texture size derived from the window dimensions. as opposed to the window
   // dimensions this one takes the view dimensions into regard and preserves an integer multiplier
   const auto ratio_width = game_config._video_mode_width / game_config._view_width;
   const auto ratio_height = game_config._video_mode_height / game_config._view_height;
   const auto size_ratio = std::min(ratio_width, ratio_height);
   _view_to_texture_scale = 1.0f / size_ratio;

   const auto texture_width = static_cast<int32_t>(size_ratio * game_config._view_width);
   const auto texture_height = static_cast<int32_t>(size_ratio * game_config._view_height);

   try
   {
      const auto texture_size = sf::Vector2u{static_cast<uint32_t>(texture_width), static_cast<uint32_t>(texture_height)};
      _render_texture_level_background = std::make_shared<sf::RenderTexture>(texture_size);
      _render_texture_level = std::make_shared<sf::RenderTexture>(texture_size, stencil_context_settings);
      _render_texture_lighting = std::make_shared<sf::RenderTexture>(texture_size, stencil_context_settings);
      _render_texture_normal = std::make_shared<sf::RenderTexture>(texture_size);
      _render_texture_normal_tmp = std::make_shared<sf::RenderTexture>(texture_size);
      _render_texture_deferred = std::make_shared<sf::RenderTexture>(texture_size);
   }
   catch (const std::exception& e)
   {
      Log::Fatal() << "failed to create render textures: " << e.what();
   }

   _atmosphere_shader = std::make_unique<AtmosphereShader>(texture_width, texture_height);
   _gamma_shader = std::make_unique<GammaShader>();
   _blur_shader = std::make_unique<BlurShader>(texture_width, texture_height);

   // keep track of those textures
   _render_textures.clear();
   _render_textures.push_back(_render_texture_level);
   _render_textures.push_back(_render_texture_level_background);
   _render_textures.push_back(_render_texture_lighting);
   _render_textures.push_back(_render_texture_normal);
   _render_textures.push_back(_render_texture_normal_tmp);
   _render_textures.push_back(_render_texture_deferred);

   for (const auto& texture : _render_textures)
   {
      Log::Info() << "created render texture: " << texture->getSize().x << " x " << texture->getSize().y;
   }

   _atmosphere_shader->initialize();
   _gamma_shader->initialize();
   _blur_shader->initialize();
}

Level::Level() : GameNode(nullptr)
{
   setClassName(typeid(Level).name());

   _ambient_occlusion = std::make_unique<AmbientOcclusion>();
   _volume_updater = std::make_unique<VolumeUpdater>();

   // init world for this level
   const b2Vec2 gravity(0.f, PhysicsConfiguration::getInstance()._gravity);

   LuaInterface::instance().reset();

   // clear those here so the world destructor doesn't double-delete them
   Projectile::clear();

   _world = std::make_shared<b2World>(gravity);

   GameContactListener::getInstance().reset();
   _world->SetContactListener(&GameContactListener::getInstance());

   __current_level = this;

   _light_system = std::make_shared<LightSystem>();

   // add raycast light for player
   if (Tweaks::instance()._player_light_enabled)
   {
      _player_light = LightSystem::createLightInstance(Player::getCurrent(), {});
      _player_light->_color = sf::Color(255, 255, 255, Tweaks::instance()._player_light_alpha);
      _light_system->_lights.push_back(_player_light);
   }

   _mechanisms_list = {
      &_mechanism_blocking_rects,
      &_mechanism_bouncers,
      &_mechanism_box_colliders,
      &_mechanism_bubble_cubes,
      &_mechanism_button_rects,
      &_mechanism_checkpoints,
      &_mechanism_collapsing_platforms,
      &_mechanism_controller_help,
      &_mechanism_conveyor_belts,
      &_mechanism_crushers,
      &_mechanism_damage_rects,
      &_mechanism_death_blocks,
      &_mechanism_dialogues,
      &_mechanism_doors,
      &_mechanism_dust,
      &_mechanism_enemy_walls,
      &_mechanism_extras,
      &_mechanism_fans,
      &_mechanism_fireflies,
      &_mechanism_info_overlay,
      &_mechanism_interaction_help,
      &_mechanism_lasers,
      &_mechanism_levers,
      &_mechanism_moveable_boxes,
      &_mechanism_on_off_blocks,
      &_mechanism_platforms,
      &_mechanism_portals,
      &_mechanism_ropes,
      &_mechanism_rotating_blades,
      &_mechanism_sensor_rects,
      &_mechanism_shader_layers,
      &_mechanism_sound_emitters,
      &_mechanism_smoke_effect,
      &_mechanism_spike_balls,
      &_mechanism_spike_blocks,
      &_mechanism_spikes,
      &_mechanism_static_lights,
      &_mechanism_text_layers,
      &_mechanism_treasure_chests,
      &_mechanism_water_damage,
      &_mechanism_water_surface,
      &_mechanism_weather,
      &_mechanism_zoomrects,
   };

   _mechanisms_map[std::string{layer_name_blocking_rects}] = &_mechanism_blocking_rects;
   _mechanisms_map[std::string{layer_name_bouncers}] = &_mechanism_bouncers;
   _mechanisms_map[std::string{layer_name_box_colliders}] = &_mechanism_box_colliders;
   _mechanisms_map[std::string{layer_name_bubble_cube}] = &_mechanism_bubble_cubes;
   _mechanisms_map[std::string{layer_name_button_rects}] = &_mechanism_button_rects;
   _mechanisms_map[std::string{layer_name_checkpoints}] = &_mechanism_checkpoints;
   _mechanisms_map[std::string{layer_name_collapsing_platforms}] = &_mechanism_collapsing_platforms;
   _mechanisms_map[std::string{layer_name_controller_help}] = &_mechanism_controller_help;
   _mechanisms_map[std::string{layer_name_conveyorbelts}] = &_mechanism_conveyor_belts;
   _mechanisms_map[std::string{layer_name_crushers}] = &_mechanism_crushers;
   _mechanisms_map[std::string{layer_name_damage_rects}] = &_mechanism_damage_rects;
   _mechanisms_map[std::string{layer_name_death_blocks}] = &_mechanism_death_blocks;
   _mechanisms_map[std::string{layer_name_dialogues}] = &_mechanism_dialogues;
   _mechanisms_map[std::string{layer_name_doors}] = &_mechanism_doors;
   _mechanisms_map[std::string{layer_name_dust}] = &_mechanism_dust;
   _mechanisms_map[std::string{layer_name_enemy_walls}] = &_mechanism_enemy_walls;
   _mechanisms_map[std::string{layer_name_extras}] = &_mechanism_extras;
   _mechanisms_map[std::string{layer_name_fans}] = &_mechanism_fans;
   _mechanisms_map[std::string{layer_name_fireflies}] = &_mechanism_fireflies;
   _mechanisms_map[std::string{layer_name_info_overlays}] = &_mechanism_info_overlay;
   _mechanisms_map[std::string{layer_name_interaction_help}] = &_mechanism_interaction_help;
   _mechanisms_map[std::string{layer_name_interval_spikes}] = &_mechanism_spikes;
   _mechanisms_map[std::string{layer_name_lasers}] = &_mechanism_lasers;
   _mechanisms_map[std::string{layer_name_levers}] = &_mechanism_levers;
   _mechanisms_map[std::string{layer_name_moveable_objects}] = &_mechanism_moveable_boxes;
   _mechanisms_map[std::string{layer_name_on_off_blocks}] = &_mechanism_on_off_blocks;
   _mechanisms_map[std::string{layer_name_platforms}] = &_mechanism_platforms;
   _mechanisms_map[std::string{layer_name_portals}] = &_mechanism_portals;
   _mechanisms_map[std::string{layer_name_ropes}] = &_mechanism_ropes;
   _mechanisms_map[std::string{layer_name_rotating_blades}] = &_mechanism_rotating_blades;
   _mechanisms_map[std::string{layer_name_sensor_rects}] = &_mechanism_sensor_rects;
   _mechanisms_map[std::string{layer_name_shader_quads}] = &_mechanism_shader_layers;
   _mechanisms_map[std::string{layer_name_smoke_effect}] = &_mechanism_smoke_effect;
   _mechanisms_map[std::string{layer_name_sound_emitters}] = &_mechanism_sound_emitters;
   _mechanisms_map[std::string{layer_name_spike_balls}] = &_mechanism_spike_balls;
   _mechanisms_map[std::string{layer_name_spike_blocks}] = &_mechanism_spike_blocks;
   _mechanisms_map[std::string{layer_name_static_lights}] = &_mechanism_static_lights;
   _mechanisms_map[std::string{layer_name_text_layer}] = &_mechanism_text_layers;
   _mechanisms_map[std::string{layer_name_treasure_chests}] = &_mechanism_treasure_chests;
   _mechanisms_map[std::string{layer_name_water_damage}] = &_mechanism_water_damage;
   _mechanisms_map[std::string{layer_name_water_surface}] = &_mechanism_water_surface;
   _mechanisms_map[std::string{layer_name_weather}] = &_mechanism_weather;
   _mechanisms_map[std::string{layer_name_zoom_rects}] = &_mechanism_zoomrects;
}

Level::~Level()
{
   Log::Info() << "deleting current level";

   // stop active timers because their callbacks being called after destruction of the level/world can be nasty
   for (const auto& enemy : LuaInterface::instance().getObjectList())
   {
      Timer::removeByCaller(enemy);
   }

   _file_watcher_thread_active = false;
   if (_file_watcher_thread.joinable())
   {
      _file_watcher_thread.join();
   }
}

// assign room identifiers to mechanism
// for now it's safe to assume that a mechanism always stays in the same room
void Level::assignMechanismsToRooms()
{
   auto update_room = [this](const std::shared_ptr<GameMechanism>& mechanism)
   {
      auto game_node = std::dynamic_pointer_cast<GameNode>(mechanism);
      if (mechanism->getBoundingBoxPx().has_value())
      {
         auto rooms = Room::findAll(mechanism->getBoundingBoxPx().value(), _rooms);
         for (const auto& room : rooms)
         {
            mechanism->addRoomId(room->_id);
         }
      }
   };

   for (auto& mechanism_vector : _mechanisms_list)
   {
      for (auto& mechanism : *mechanism_vector)
      {
         update_room(mechanism);
      }
   }

   for (const auto& enemy : LuaInterface::instance().getObjectList())
   {
      update_room(enemy);
   }
}

void Level::loadTmx()
{
   static const std::string parallax_identifier = "parallax_";
   const auto path = std::filesystem::path(_description->_filename).parent_path();

#ifdef DEVELOPMENT_MODE
   // checksum checking won't be needed in production
   const auto checksum_old = Checksum::readChecksum(_description->_filename + ".crc");
   const auto checksum_new = Checksum::calcChecksum(_description->_filename);
   const auto checksum_mismatch = checksum_old != checksum_new;

   if (checksum_mismatch || _loading_mode == LoadingMode::Clean)
   {
      LevelFiles::clean(*_description);
      Checksum::writeChecksum(_description->_filename + ".crc", checksum_new);
   }
#endif

   sf::Clock elapsed;

   // parse tmx
   TmxParser tmx_parser;
   Log::Info() << "parsing tmx: " << _description->_filename;
   tmx_parser.parse(_description->_filename);
   Log::Info() << "parsing tmx, done within " << elapsed.getElapsedTime().asSeconds() << "s";

   Log::info("loading tmx... ");
   elapsed.restart();

   GameDeserializeData data;
   data._world = _world;
   data._base_path = path;

   GameMechanismDeserializer::deserialize(tmx_parser, this, data, _mechanisms_map);

   // preload mechanism data
   for (auto& [vec_key, vec_values] : _mechanisms_map)
   {
      for (auto& val : *vec_values)
      {
         val->preload();
      }
   }

   // process everything that's not considered a mechanism
   const auto& tmx_elements = tmx_parser.getElements();
   for (const auto& element : tmx_elements)
   {
      data._tmx_layer = nullptr;
      data._tmx_tileset = nullptr;
      data._tmx_object = nullptr;
      data._tmx_object_group = nullptr;

      // parse sprites
      if (element->_type == TmxElement::Type::TypeLayer)
      {
         auto layer = std::dynamic_pointer_cast<TmxLayer>(element);
         if (GameMechanismDeserializer::isLayerNameReserved(layer->_name))
         {
            continue;
         }

         const auto tileset = tmx_parser.getTileSet(layer);

         data._tmx_layer = layer;
         data._tmx_tileset = tileset;

         const auto tile_map = TileMapFactory::makeTileMap(layer);
         tile_map->load(layer, tileset, path);
         auto push_tile_map = true;

         if (layer->_name == "atmosphere")
         {
            _atmosphere._tile_map = tile_map;
            _atmosphere.parse(layer, tileset);
         }
         else if (layer->_name.compare(0, parallax_identifier.length(), parallax_identifier) == 0)
         {
            auto parallax_layer = ParallaxLayer::deserialize(layer, tile_map);
            _parallax_layers.push_back(std::move(parallax_layer));
            push_tile_map = false;
         }
         else if (layer->_name == "level" || layer->_name == "level_solid_onesided" || layer->_name == "level_deadly")
         {
            parsePhysicsTiles(layer, tileset, path);
         }

         if (push_tile_map)
         {
            _tile_maps.push_back(tile_map);
         }
      }

      // parse objects
      else if (element->_type == TmxElement::Type::TypeObjectGroup)
      {
         const auto object_group = std::dynamic_pointer_cast<TmxObjectGroup>(element);

         for (const auto& object : object_group->_objects)
         {
            const auto tmx_object = object.second;
            data._tmx_object = tmx_object;
            data._tmx_object_group = object_group;

            if (object_group->_name == "enemies")
            {
               TmxEnemy enemy;
               enemy.parse(tmx_object);
               _enemy_data_from_tmx_layer[enemy._id] = enemy;
            }
            else if (object_group->_name == "rooms")
            {
               Room::deserialize(this, data, _rooms);
            }
            else if (object_group->_name == "lights")
            {
               const auto light = LightSystem::createLightInstance(this, data);
               _light_system->_lights.push_back(light);
            }
         }
      }

      // parse images
      else if (element->_type == TmxElement::Type::TypeImageLayer)
      {
         const auto image = ImageLayer::deserialize(element, path);
         _image_layers.push_back(image);
      }
   }

   TileMapFactory::merge(_tile_maps);
   Room::mergeEnterAreas(_rooms);

   if (!_atmosphere._tile_map)
   {
      Log::Error() << "fatal: no physics layer (called 'physics') found!";
   }

   Log::Info() << "loading tmx, done within " << elapsed.getElapsedTime().asSeconds() << "s";
}

BoomEffect& Level::getBoomEffect()
{
   return _boom_effect;
}

bool Level::load()
{
   const auto level_json_path = std::filesystem::path(_description->_filename);
   setObjectId(_description->_filename);

   if (!std::filesystem::exists(level_json_path))
   {
      Log::Error() << "path " << level_json_path << " does not exist";
      return false;
   }

   loadTmx();

   _ambient_occlusion->load(level_json_path.parent_path(), std::filesystem::path(_description->_filename).stem().string());

   Log::Info() << "level loading complete";

   // set up file watcher
   _file_watcher_thread = std::thread(
      [this]()
      {
         auto first_modified_time = std::filesystem::last_write_time(_description->_filename);

         while (_file_watcher_thread_active)
         {
            const auto current_modified_time = std::filesystem::last_write_time(_description->_filename);
            if (current_modified_time != first_modified_time)
            {
               Log::Info() << "level was modified, marking as dirty";
               first_modified_time = current_modified_time;
               _dirty = true;
            }

            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);
         }
      }
   );

   return true;
}

void Level::loadStartPosition()
{
   _start_position.x = static_cast<float_t>(_description->_start_position.at(0) * PIXELS_PER_TILE + PLAYER_ACTUAL_WIDTH / 2);
   _start_position.y = static_cast<float_t>(_description->_start_position.at(1) * PIXELS_PER_TILE + DIFF_PLAYER_TILE_TO_PHYSICS);
}

void Level::loadLevelScript()
{
   const auto path = std::filesystem::path(_description->_filename).parent_path();
   _level_script.setup(path / "level.lua");
   _level_script.setSearchMechanismCallback([this](const std::string& regexPattern, const std::optional<std::string>& group)
                                            { return searchMechanisms(regexPattern, group); });

   _level_script.createExtraCallbacks(_mechanism_extras);
}

void Level::initialize()
{
   createViews();

   _description = LevelDescription::load(_description_filename);

   if (!_description)
   {
      Log::Error() << "level configuration is bad, could not load " << _description_filename;
      return;
   }

   if (!load())
   {
      Log::Error() << "level loading failed, filename: " << _description_filename;
      return;
   }

   loadStartPosition();

   loadSaveState();
   spawnEnemies();

   assignMechanismsToRooms();
   _volume_updater->setMechanisms(_mechanisms_list);

   loadLevelScript();

   // dump();
}

void Level::loadSaveState()
{
   const auto& save_state = SaveState::getCurrent();
   auto checkpoint_index = save_state._checkpoint;
   auto checkpoint = Checkpoint::getCheckpoint(checkpoint_index, _mechanism_checkpoints);

   if (checkpoint)
   {
      auto pos = checkpoint->spawnPoint();
      _start_position.x = static_cast<float>(pos.x);
      _start_position.y = static_cast<float>(pos.y);
      Log::Info() << "move to checkpoint: " << checkpoint_index;
   }
   else
   {
      Log::Error() << "level doesn't have a start check point set up";
   }

   if (save_state._level_state.is_null())
   {
      return;
   }

   const auto& level_json = save_state._level_state[_description->_filename];
   if (level_json.is_null())
   {
      return;
   }

   for (auto& [mechanism_key, mechanism_values] : level_json.items())
   {
      const auto& mechanism_it = _mechanisms_map.find(mechanism_key);
      if (mechanism_it == _mechanisms_map.end())
      {
         continue;
      }

      const auto& mechanism_vector = mechanism_it->second;
      for (auto& [object_key, object_value] : mechanism_values.items())
      {
         auto result = std::find_if(
            mechanism_vector->begin(),
            mechanism_vector->end(),
            [object_key](const auto& object)
            {
               auto* game_node = dynamic_cast<GameNode*>(object.get());
               return (game_node && game_node->getObjectId() == object_key);
            }
         );

         if (result != mechanism_vector->end())
         {
            (*result)->deserializeState(object_value);
         }
      }
   }
}

void Level::saveState()
{
   auto& j = SaveState::getCurrent()._level_state;

   nlohmann::json mechanisms_json;

   // serialize the states of all mechanisms
   for (auto& [key, mechanisms] : _mechanisms_map)
   {
      nlohmann::json mechanism_json;
      for (auto& mechanism : *mechanisms)
      {
         mechanism->serializeState(mechanism_json);
      }

      if (!mechanism_json.empty())
      {
         mechanisms_json[key] = mechanism_json;
      }
   }

   j[_description->_filename] = mechanisms_json;
}

void Level::reset()
{
   for (auto& door : _mechanism_doors)
   {
      door.reset();
   }
}

void Level::spawnEnemies()
{
   // those enemies that have a lua script associated inside the tmx layer don't need
   // additional information from json, those can just be spawned.
   // this should probably be the future and only approach how to handle enemy spawning.
   for (auto& it : _enemy_data_from_tmx_layer)
   {
      const auto script = it.second.findProperty("script");

      if (!script.has_value())
      {
         Log::Error() << "missing script definition";
         continue;
      }

      auto lua_node = LuaInterface::instance().addObject(this, std::string("data/scripts/enemies/") + script.value()._value);

      EnemyDescription json_description;
      json_description._position_in_tiles = false;
      json_description._start_position.push_back(it.second._pixel_position.x);
      json_description._start_position.push_back(it.second._pixel_position.y);
      json_description._id = it.second._id;
      json_description._name = it.second._name;

      const auto& generate_path_property = it.second.findProperty("generate_path");
      const auto& inverse_path_property = it.second.findProperty("inverse_path");
      if (generate_path_property.has_value() && generate_path_property.value()._value == "1")
      {
         it.second._inverse_path = (inverse_path_property.has_value() && inverse_path_property.value()._value == "1");
         it.second.addPaths(_world_chains);
      }

      if (!it.second._pixel_path.empty())
      {
         json_description._path = it.second._pixel_path;
      }

      // z index
      const auto& z_index_property = it.second.findProperty("z");
      if (z_index_property.has_value())
      {
         lua_node->setZ(std::stoi(z_index_property.value()._value));
      }

      // merge properties from tmx with those loaded from json
      for (auto& property : it.second._properties)
      {
         json_description._properties.push_back(property);
      }

      // initialize lua node and store enemy
      lua_node->_enemy_description = json_description;
      lua_node->initialize();
   }
}

void Level::drawStaticChains(sf::RenderTarget& target)
{
   for (auto& path : _atmosphere._outlines)
   {
      target.draw(&path.at(0), path.size(), sf::PrimitiveType::LineStrip);
   }
}

const std::shared_ptr<sf::View>& Level::getLevelView() const
{
   return _level_view;
}

void Level::createViews()
{
   auto& game_config = GameConfiguration::getInstance();

   // the view dimensions never change
   _view_width = static_cast<float>(game_config._view_width);
   _view_height = static_cast<float>(game_config._view_height);

   _level_view.reset();
   _level_view = std::make_shared<sf::View>(sf::FloatRect({0.0f, 0.0f}, {_view_width, _view_height}));
   _level_view->setViewport(sf::FloatRect({0.0f, 0.0f}, {1.0f, 1.0f}));

   for (auto& parallax_layer : _parallax_layers)
   {
      parallax_layer->resetView(_view_width, _view_height);
   }

   for (auto& image_layer : _image_layers)
   {
      image_layer->resetView(_view_width, _view_height);
   }
}

void Level::updateViews()
{
   // this should really just fetch the camera position and the camera panorama vectors and
   // update the views with them; no camera or camera panorama correction must be done here
   const auto& look_vector = CameraPanorama::getInstance().getLookVector();
   const auto& camera_system = CameraSystem::getInstance();
   const auto level_view_x = camera_system.getX() + look_vector.x;
   const auto level_view_y = camera_system.getY() + look_vector.y;

   auto& zoom = CameraZoom::getInstance();
   auto view_rect = sf::FloatRect{{level_view_x, level_view_y}, {_view_width, _view_height}};
   zoom.adjust(view_rect);

   CameraRoomLock::setViewRect(view_rect);

   _level_view = std::make_shared<sf::View>(view_rect);

   for (const auto& parallax : _parallax_layers)
   {
      parallax->updateView(view_rect.position.x, view_rect.position.y, view_rect.size.x, view_rect.size.y);
   }

   for (const auto& image_layer : _image_layers)
   {
      image_layer->updateView(view_rect.position.x, view_rect.position.y, view_rect.size.x, view_rect.size.y);
   }
}

void Level::updateMechanismVolumes()
{
   if (!_volume_updater)
   {
      return;
   }

   _volume_updater->setPlayerPosition(Player::getCurrent()->getPixelPositionFloat());
}

void Level::updateRoom()
{
   RoomUpdater::setCurrent(Room::find(Player::getCurrent()->getPixelPositionFloat(), _rooms));
}

void Level::syncRoom()
{
   RoomUpdater::setCurrent(Room::find(Player::getCurrent()->getPixelPositionFloat(), _rooms));
   CameraRoomLock::setRoom(RoomUpdater::getCurrent());
}

void Level::updateCameraSystem(const sf::Time& dt)
{
   auto& camera_system = CameraSystem::getInstance();

   // update room
   const auto room_previous = RoomUpdater::getCurrent();
   updateRoom();
   const auto room_current = RoomUpdater::getCurrent();

   // room changed
   if (room_previous != room_current)
   {
      std::string room_id = "undefined";
      std::string enter_area_name = "undefined";
      if (room_current)
      {
         room_id = room_current->getObjectId();
         const auto entered_area = room_current->enteredArea(Player::getCurrent()->getPixelPositionFloat());
         if (entered_area.has_value())
         {
            enter_area_name = entered_area.value()._name;
         }
      }

      Log::Info() << "player moved to room: " << room_id << " on side " << enter_area_name;

      // will update the current room in both cases, either after the camera lock delay or instantly
      if (room_current && room_current->_camera_lock_delay.has_value())
      {
         room_current->lockCamera();
      }
      else
      {
         CameraRoomLock::setRoom(room_current);
      }

      // trigger transition effect
      // when level has been loaded, room changes certainly do not require a transition
      if (RoomUpdater::isSynced() && room_current)
      {
         room_current->startTransition();
      }
   }

   // update camera system
   if (!room_current || !room_current->_camera_locked)
   {
      camera_system.update(dt, _view_width, _view_height);
   }

   RoomUpdater::setSynced(true);

   CameraZoom::getInstance().update(dt);
}

void Level::zoomIn()
{
   auto& zoom = CameraZoom::getInstance();
   zoom.setZoomFactor(zoom.getZoomFactor() * 0.95);
}

void Level::zoomOut()
{
   auto& zoom = CameraZoom::getInstance();
   zoom.setZoomFactor(zoom.getZoomFactor() * 1.05);
}

void Level::zoomBy(float delta)
{
   auto& zoom = CameraZoom::getInstance();
   zoom.setZoomFactor(std::clamp(zoom.getZoomFactor() + delta * 0.2f, 0.1f, 20.0f));
}

void Level::zoomReset()
{
   auto& zoom = CameraZoom::getInstance();
   zoom.setZoomFactor(1.0f);
}

void Level::drawLightMap()
{
   _render_texture_lighting->clear();
   _render_texture_lighting->setView(*_level_view);
   _light_system->draw(*_render_texture_lighting, {});
   _render_texture_lighting->display();

   //   static int32_t light_map_save_counter = 0;
   //   light_map_save_counter++;
   //   if (light_map_save_counter % 60 == 0)
   //   {
   //      _render_texture_lighting->getTexture().copyToImage().saveToFile("light_map.png");
   //   }
}

void Level::drawParallaxMaps(sf::RenderTarget& target, int32_t z_index)
{
   for (const auto& parallax : _parallax_layers)
   {
      if (parallax->_z_index == z_index)
      {
         target.setView(parallax->_view);
         target.draw(*parallax->_tile_map);
      }
   }

   // restore level view
   target.setView(*_level_view);
}

void Level::drawPlayer(sf::RenderTarget& color, sf::RenderTarget& normal)
{
   auto player = Player::getCurrent();
   player->draw(color, normal);
}

void Level::drawLayers(sf::RenderTarget& target, sf::RenderTarget& normal, int32_t from, int32_t to)
{
   const auto& player_chunk = Player::getCurrent()->getChunk();

   target.setView(*_level_view);
   normal.setView(*_level_view);

   for (auto z_index = from; z_index <= to; z_index++)
   {
      PlayerStencil::draw(target, z_index);
      drawParallaxMaps(target, z_index);

      // draw all tile maps
      for (const auto& tile_map : _tile_maps)
      {
         if (tile_map->getZ() == z_index)
         {
            tile_map->draw(target, normal, {});
         }
      }

      // draw mechanisms
      for (auto* mechanism_vector : _mechanisms_list)
      {
         for (const auto& mechanism : *mechanism_vector)
         {
            if (mechanism->getZ() != z_index)
            {
               continue;
            }

            if (checkUpdateMechanism(player_chunk, mechanism))
            {
               mechanism->draw(target, normal);
            }
         }
      }

      // ambient occlusion
      if (z_index == _ambient_occlusion->getZ())
      {
         _ambient_occlusion->draw(target);
      }

      // draw enemies
      for (auto& enemy : LuaInterface::instance().getObjectList())
      {
         if (enemy->getZ() == z_index)
         {
            if (checkUpdateMechanism(player_chunk, enemy))
            {
               enemy->draw(target, normal);
            }
         }
      }

      // draw player
      if (z_index == static_cast<int32_t>(ZDepth::Player))
      {
         drawPlayer(target, normal);
      }

      // draw image layers
      for (auto& layer : _image_layers)
      {
         if (layer->getZ() == z_index)
         {
            layer->draw(target, normal);
         }
      }
   }
}

void Level::drawAtmosphereLayer()
{
   if (!_atmosphere._tile_map)
   {
      return;
   }

   _atmosphere_shader->getRenderTexture()->clear();
   _atmosphere._tile_map->setVisible(true);
   _atmosphere_shader->getRenderTexture()->setView(*_level_view);
   _atmosphere_shader->getRenderTexture()->draw(*_atmosphere._tile_map);
   _atmosphere._tile_map->setVisible(false);
   _atmosphere_shader->getRenderTexture()->display();
}

void Level::drawBlurLayer(sf::RenderTarget& target)
{
   target.setView(*_level_view);

   // draw elements that are supposed to glow / to be blurred here

#ifdef GLOW_ENABLED
   // lasers have been removed here because dstar added the glow to the spriteset

   const auto pPos = Player::getCurrent()->getPixelPositionf();

   // draw lasers
   for (auto laser : mLasers)
   {
      const auto lPos = std::dynamic_pointer_cast<Laser>(laser)->getPixelPosition();
      if (SfmlMath::lengthSquared(lPos - pPos) > 250000)
      {
         continue;
      }

      laser->draw(target);
   }
#endif
}

bool Level::isPhysicsPathClear(const sf::Vector2i& a_tl, const sf::Vector2i& b_tl) const
{
   if (a_tl.x < 0 || a_tl.y < 0 || b_tl.x < 0 || b_tl.y < 0 || a_tl.x > _physics._grid_width || b_tl.x > _physics._grid_width ||
       a_tl.y > _physics._grid_height || b_tl.y > _physics._grid_height)
   {
      return false;
   }

   auto blocks = [this](uint32_t x, uint32_t y) -> bool { return _physics._physics_map[(_physics._grid_width * y) + x] == 1; };

   return MapTools::lineCollide(a_tl.x, a_tl.y, b_tl.x, b_tl.y, blocks);
}

void Level::takeScreenshot(const std::string& basename, sf::RenderTexture& texture)
{
   if (!_screenshot)
   {
      return;
   }

   std::ostringstream ss;
   ss << basename << "_" << std::setw(2) << std::setfill('0') << _screenshot_counters[basename] << ".png";
   _screenshot_counters[basename]++;
   texture.getTexture().copyToImage().saveToFile(ss.str());
}

void Level::drawDebugInformation()
{
   if (DisplayMode::getInstance().isSet(Display::Debug))
   {
      auto& texture = *_render_texture_level.get();
      drawStaticChains(texture);
      DebugDraw::debugBodies(texture, this);
      DebugDraw::drawRect(texture, Player::getCurrent()->getPixelRectFloat());
      DebugDraw::debugHitboxes(texture);

      for (const auto& room : _rooms)
      {
         for (const auto& sub_room : room->_sub_rooms)
         {
            DebugDraw::drawRect(texture, sub_room._rect, sf::Color::Yellow);
         }
      }
   }
}

void Level::displayFinalTextures()
{
   // display the whole texture
   sf::View view(sf::FloatRect(
      {0.0f, 0.0f}, {static_cast<float>(_render_texture_level->getSize().x), static_cast<float>(_render_texture_level->getSize().y)}
   ));

   view.setViewport(sf::FloatRect({0.0f, 0.0f}, {1.0f, 1.0f}));

   _render_texture_level->setView(view);
   _render_texture_level->display();

   _render_texture_normal->setView(view);
   _render_texture_normal->display();
}

void Level::drawGlowLayer()
{
#ifdef GLOW_ENABLED
   _blur_shader->clearTexture();
   drawBlurLayer(*mBlurShader->getRenderTexture().get());
   _blur_shader->getRenderTexture()->display();
   takeScreenshot("screenshot_blur", *_blur_shader->getRenderTexture().get());
#endif
}

void Level::drawGlowSprite()
{
#ifdef GLOW_ENABLED
   sf::Sprite blur_sprite(_blur_shader->getRenderTexture()->getTexture());
   const auto down_scale_x =
      _blur_shader->getRenderTextureScaled()->getSize().x / static_cast<float>(mBlurShader->getRenderTexture()->getSize().x);
   const auto down_scale_y =
      _blur_shader->getRenderTextureScaled()->getSize().y / static_cast<float>(mBlurShader->getRenderTexture()->getSize().y);
   blur_sprite.scale({down_scale_x, down_scale_y});

   sf::RenderStates statesShader;
   _blur_shader->update();
   states_shader.shader = &mBlurShader->getShader();
   _blur_shader->getRenderTextureScaled()->draw(blur_sprite, statesShader);

   sf::Sprite blur_scale_sprite(mBlurShader->getRenderTextureScaled()->getTexture());
   blur_scale_sprite.scale(1.0f / down_scale_x, 1.0f / down_scale_y);
   blur_scale_sprite.setTextureRect(sf::IntRect(
      0,
      static_cast<int32_t>(blur_scale_sprite.getTexture()->getSize().y),
      static_cast<int32_t>(blur_scale_sprite.getTexture()->getSize().x),
      -static_cast<int32_t>(blur_scale_sprite.getTexture()->getSize().y)
   ));

   sf::RenderStates states_add;
   statesAdd.blendMode = sf::BlendAdd;
   _level_render_texture->draw(blur_scale_sprite, states_add);
#endif
}

std::vector<std::shared_ptr<GameMechanism>>
Level::searchMechanisms(const std::string& regex_pattern, const std::optional<std::string>& group)
{
   std::vector<std::shared_ptr<GameMechanism>> results;

   std::regex pattern(regex_pattern);
   for (const auto& [key, mechanism_vector] : _mechanisms_map)
   {
      // filter by mechanism group if requested
      if (group.has_value() && group.value() != key)
      {
         continue;
      }

      for (const auto& mechanism : *mechanism_vector)
      {
         auto node = std::dynamic_pointer_cast<GameNode>(mechanism);
         if (std::regex_match(node->getObjectId(), pattern))
         {
            results.push_back(mechanism);
         }
      }
   }

   // also scan image layers since those are separate at the moment
   if (!group.has_value() || group.value() == "imagelayers")
   {
      for (const auto& image_layer : _image_layers)
      {
         auto node = std::dynamic_pointer_cast<GameNode>(image_layer);
         if (std::regex_match(node->getObjectId(), pattern))
         {
            results.push_back(image_layer);
         }
      }
   }

   return results;
}

void Level::setLoadingMode(LoadingMode loading_mode)
{
   _loading_mode = loading_mode;
}

bool Level::isDirty() const
{
   return _dirty;
}

const std::vector<std::shared_ptr<GameMechanism>>& Level::getBouncers() const
{
   return _mechanism_bouncers;
}

const std::vector<std::shared_ptr<GameMechanism>>& Level::getPortals() const
{
   return _mechanism_portals;
}

const std::vector<std::shared_ptr<GameMechanism>>& Level::getExtras() const
{
   return _mechanism_extras;
}

// Level Rendering Flow
//
//    textures/render targets:
//    - atmosphere texture
//    - level background texture
//    - level texture
//    - window
//
//    01) draw atmosphere (air / water)                           -> atmosphere texture
//    02) draw parallax info                                      -> level background texture
//    03) draw level background                                   -> level texture
//        - layers z=0..15
//    04) draw level background normals                           -> normal tmp texture
//        - layers z=0..15
//    05) draw level background with atmosphere shader on         -> background texture
//    06) draw level background normals with atmosphere shader on -> normal texture
//    07) draw level foreground                                   -> level texture
//        - layers z=16..50
//        - additive lights
//        - mechanisms
//        - ambient occlusion
//        - images with varying blend modes
//        - player
//    08) draw raycast lights                                     -> level texture
//    09) draw projectiles                                        -> level texture
//    10) flash and bounce -> move level texture
//    11) draw level texture with gamma shader enabled            -> straight to window
//    12) draw level map (if enabled)                             -> straight to window
//
void Level::draw(const std::shared_ptr<sf::RenderTexture>& window, bool screenshot)
{
   _screenshot = screenshot;

   // render atmosphere to atmosphere texture, that texture is used in the shader only
   drawAtmosphereLayer();
   takeScreenshot("texture_atmosphere", *_atmosphere_shader->getRenderTexture().get());

   // render glowing elements
   drawGlowLayer();

   // render layers affected by the atmosphere
   _render_texture_level->clear();
   _render_texture_level_background->clear();
   _render_texture_normal_tmp->clear();
   _render_texture_normal->clear();

   drawLayers(
      *_render_texture_level_background.get(),
      *_render_texture_normal_tmp.get(),
      static_cast<int32_t>(ZDepth::BackgroundMin),
      static_cast<int32_t>(ZDepth::BackgroundMax)
   );

   _render_texture_level_background->display();
   takeScreenshot("texture_level_background", *_render_texture_level_background.get());

   _render_texture_normal_tmp->display();
   takeScreenshot("texture_level_background_normal", *_render_texture_normal_tmp.get());

   // draw the atmospheric parts into the level texture using the atmosphere shader
   sf::Sprite tmp_sprite(_render_texture_level_background->getTexture());
   _atmosphere_shader->update();
   _render_texture_level->draw(tmp_sprite, &_atmosphere_shader->getShader());
   takeScreenshot("texture_level_level_dist", *_render_texture_level.get());

   // draw the normal parts into the final normal texture using the atmosphere shader
   tmp_sprite.setTexture(_render_texture_normal_tmp->getTexture());
   _atmosphere_shader->update();
   _render_texture_normal->draw(tmp_sprite, &_atmosphere_shader->getShader());
   takeScreenshot("texture_level_background_normal_dist", *_render_texture_normal.get());

   drawGlowSprite();

   // draw the level layers into the level texture
   drawLayers(
      *_render_texture_level.get(),
      *_render_texture_normal.get(),
      static_cast<int32_t>(ZDepth::ForegroundMin),
      static_cast<int32_t>(ZDepth::ForegroundMax)
   );

   Gun::drawProjectileHitAnimations(*_render_texture_level.get());
   AnimationPlayer::getInstance().draw(*_render_texture_level.get());

   drawDebugInformation();

   displayFinalTextures();

   drawLightMap();

   _light_system->draw(*_render_texture_deferred.get(), _render_texture_level, _render_texture_lighting, _render_texture_normal);

   _render_texture_deferred->display();

   takeScreenshot("texture_map_color", *_render_texture_level.get());
   takeScreenshot("texture_map_light", *_render_texture_lighting.get());
   takeScreenshot("texture_map_normal", *_render_texture_normal.get());
   takeScreenshot("texture_map_deferred", *_render_texture_deferred.get());

   auto level_texture_sprite = sf::Sprite(_render_texture_deferred->getTexture());
   _gamma_shader->setTexture(_render_texture_deferred->getTexture());

   level_texture_sprite.setPosition({_boom_effect._boom_offset_x, _boom_effect._boom_offset_y});
   level_texture_sprite.scale({_view_to_texture_scale, _view_to_texture_scale});

   _gamma_shader->update();
   window->draw(level_texture_sprite, &_gamma_shader->getGammaShader());
}

void Level::updatePlayerLight()
{
   if (!Tweaks::instance()._player_light_enabled)
   {
      return;
   }

   _player_light->_pos_m = Player::getCurrent()->getBody()->GetPosition();
   _player_light->updateSpritePosition();

   // the player, once he dies, becomes inactive and just sinks down
   // so the player light is disabled to avoid any glitches
   _player_light->_color = sf::Color(255, 255, 255, Player::getCurrent()->isDead() ? 0 : Tweaks::instance()._player_light_alpha);
}

const std::shared_ptr<LightSystem>& Level::getLightSystem() const
{
   return _light_system;
}

void Level::update(const sf::Time& dt)
{
   Projectile::update(dt);

   updateMechanismVolumes();
   updateCameraSystem(dt);
   updateViews();

   // clear conveyor belt state BEFORE updating the world
   // i.e. all objects on the belt are cleared here, then in Step() they are re-collected
   ConveyorBelt::resetBeltState();

   _world->Step(PhysicsConfiguration::getInstance()._time_step, 8, 3);

   CameraPanorama::getInstance().update();
   _boom_effect.update(dt);

   AnimationPlayer::getInstance().update(dt);

   for (auto& tile_map : _tile_maps)
   {
      tile_map->update(dt);
   }

   const auto& player_chunk = Player::getCurrent()->getChunk();

   for (auto* mechanism_vector : _mechanisms_list)
   {
      for (const auto& mechanism : *mechanism_vector)
      {
         if (checkUpdateMechanism(player_chunk, mechanism))
         {
#ifdef MECHANISM_TIMING_ENABLED
            auto game_node = dynamic_cast<GameNode*>(mechanism.get());
            const auto class_name = game_node ? game_node->getClassName() : "unknown";
            const auto time_start = std::chrono::high_resolution_clock::now();
#endif
            mechanism->update(dt);

#ifdef MECHANISM_TIMING_ENABLED
            const auto time_end = std::chrono::high_resolution_clock::now();
            timing_data[class_name].addUpdateTime(time_end - time_start);
#endif
         }
      }
   }

   for (auto& layer : _image_layers)
   {
      layer->update(dt);
   }

#ifdef MECHANISM_TIMING_ENABLED
   static auto timing_debug_counter = 0;
   if (timing_debug_counter % 1000 == 0)
   {
      logUpdateTimes(timing_data, 5);
      timing_data.clear();
   }

   timing_debug_counter++;
#endif

   _level_script.update(dt);

   LuaInterface::instance().update(
      dt, [&player_chunk](const std::shared_ptr<GameMechanism>& mechanism) { return checkUpdateMechanism(player_chunk, mechanism); }
   );

   updatePlayerLight();

   _volume_updater->setRoomId(RoomUpdater::getCurrentId());
   _volume_updater->update();
   _volume_updater->updateProjectiles(Projectile::getProjectiles());
}

const std::shared_ptr<b2World>& Level::getWorld() const
{
   return _world;
}

void Level::addChainToWorld(const std::vector<b2Vec2>& chain, ObjectType object_type)
{
   if (fabs(chain[0].x - chain[chain.size() - 1].x) < 0.001f && fabs(chain[0].y - chain[chain.size() - 1].y) < 0.001f)
   {
      Log::Error() << "chain has equal start and end position" << std::endl;
      exit(0);
   }

   // it's easier to store all the physics chains in a separate data structure
   // than to parse the box2d world every time we want those loops.
   _world_chains.push_back(chain);

   b2ChainShape chain_shape;
   chain_shape.CreateLoop(&chain.at(0), static_cast<int32_t>(chain.size()));

   b2FixtureDef fixture_def;
   fixture_def.density = 0.0f;
   fixture_def.friction = 0.2f;
   fixture_def.shape = &chain_shape;

   b2BodyDef body_def;
   body_def.position.Set(0, 0);
   body_def.type = b2_staticBody;

   auto body = _world->CreateBody(&body_def);
   auto fixture = body->CreateFixture(&fixture_def);
   auto object_data = new FixtureNode(this);
   object_data->setObjectId(std::format("world_chain_{}", _world_chains.size() - 1));
   object_data->setType(object_type);
   fixture->SetUserData(static_cast<void*>(object_data));
}

void Level::addPathsToWorld(int32_t offset_x, int32_t offset_y, const std::vector<SquareMarcher::Path>& paths, ObjectType behavior)
{
   // create the physical chain with 1 body per chain
   for (const auto& path : paths)
   {
      std::vector<b2Vec2> chain(path._scaled.size());

      std::transform(
         path._scaled.begin(),
         path._scaled.end(),
         chain.begin(),
         [&](const auto& pos) { return b2Vec2((pos.x + offset_x) * PIXELS_PER_TILE / PPM, (pos.y + offset_y) * PIXELS_PER_TILE / PPM); }
      );

      addChainToWorld(chain, behavior);
   }
}

void Level::parseObj(const std::shared_ptr<TmxLayer>& layer, ObjectType behavior, const std::filesystem::path& path)
{
   std::vector<b2Vec2> points;
   std::vector<std::vector<uint32_t>> faces;
   Mesh::readObj(path.string(), points, faces);
   for (const auto& face : faces)
   {
      std::vector<b2Vec2> chain;
      std::vector<sf::Vector2f> debug_path;
      for (auto index : face)
      {
         const auto& p = points[index];
         const auto v = b2Vec2{(p.x + layer->_offset_x_px) / PPM, (p.y + layer->_offset_y_px) / PPM};

         if (_winding == Winding::Clockwise)
         {
            // box2D 2.4 changed the winding to clockwise, anything CCW will be fall-through
            chain.insert(chain.begin(), v);
         }
         else
         {
            chain.push_back(v);
         }

         debug_path.emplace_back(p.x / PIXELS_PER_TILE, p.y / PIXELS_PER_TILE);
      }

      // creating a box2d loop is automatically closing the path
      chain.pop_back();
      addChainToWorld(chain, behavior);

      // this should become a function callable from the console
      if (false)
      {
         Mesh::writeVerticesToImage(
            points,
            faces,
            {static_cast<int32_t>(layer->_width_tl * PIXELS_PER_TILE) / 4, static_cast<int32_t>(layer->_height_tl * PIXELS_PER_TILE) / 4},
            path.filename().string() + "_dump.png",
            0.25f
         );
      }
   }
}

void Level::regenerateLevelPaths(
   const std::shared_ptr<TmxLayer>& layer,
   const std::shared_ptr<TmxTileSet>& tileset,
   const std::filesystem::path& base_path,
   ParseData* parse_data,
   auto path_solid_optimized
)
{
   const auto path_solid_not_optimized = base_path / std::filesystem::path(parse_data->filename_obj_not_optimized);

   // dump the tileset into an obj file, optimise that and load it
   if (_physics.dumpObj(layer, tileset, path_solid_not_optimized))
   {
#ifdef __linux__
      auto cmd = std::string("tools/path_merge/path_merge") + " " + path_solid_not_optimized.string() + " " + path_solid_optimized.string();
#elif defined __APPLE__
      auto cmd =
         std::string("tools/path_merge/path_merge_macos") + " " + path_solid_not_optimized.string() + " " + path_solid_optimized.string();
#else
      auto cmd =
         std::string("tools\\path_merge\\path_merge.exe") + " " + path_solid_not_optimized.string() + " " + path_solid_optimized.string();
#endif

      Log::Info() << "running cmd: " << cmd;

      if (std::system(cmd.c_str()) != 0)
      {
         Log::Error() << "command failed";
      }
      else
      {
         Log::Info() << "command succeeded";
      }
   }
   else
   {
      Log::Error() << "dumping unoptimized obj (" << path_solid_not_optimized << ") failed";
   }

   parseObj(layer, parse_data->object_type, path_solid_optimized);
}

void Level::parsePhysicsTiles(
   const std::shared_ptr<TmxLayer>& layer,
   const std::shared_ptr<TmxTileSet>& tileset,
   const std::filesystem::path& base_path
)
{
   ParseData level_pd;
   level_pd.filename_obj_optimized = "layer_" + layer->_name + "_solid.obj";
   level_pd.filename_obj_not_optimized = "layer_" + layer->_name + "_solid_not_optimised.obj";
   level_pd.filename_physics_path_csv = "physics_path_solid.csv";
   level_pd.filename_grid_image = "physics_grid_solid.png";
   level_pd.filename_path_image = "physics_path_solid.png";
   level_pd.object_type = ObjectTypeSolid;
   level_pd.colliding_tiles = {1};

   ParseData solid_onesided_pd;
   solid_onesided_pd.filename_obj_optimized = "layer_" + layer->_name + "_solid_onesided.obj";
   solid_onesided_pd.filename_obj_not_optimized = "layer_" + layer->_name + "_solid_onesided_not_optimised.obj";
   solid_onesided_pd.filename_physics_path_csv = "physics_path_solid_onesided.csv";
   solid_onesided_pd.filename_grid_image = "physics_grid_solid_onesided.png";
   solid_onesided_pd.filename_path_image = "physics_path_solid_onesided.png";
   solid_onesided_pd.object_type = ObjectTypeSolidOneWay;
   solid_onesided_pd.colliding_tiles = {1};

   ParseData* parse_data = nullptr;

   if (layer->_name == "level")
   {
      parse_data = &level_pd;
   }
   else if (layer->_name == "level_solid_onesided")
   {
      parse_data = &solid_onesided_pd;
   }

   if (!parse_data)
   {
      return;
   }

   auto path_solid_optimized = base_path / std::filesystem::path(parse_data->filename_obj_optimized);

   Log::Info() << "loading: " << path_solid_optimized.make_preferred().generic_string();

   if (std::filesystem::exists(path_solid_optimized))
   {
      parseObj(layer, parse_data->object_type, path_solid_optimized);
   }
   else
   {
      regenerateLevelPaths(layer, tileset, base_path, parse_data, path_solid_optimized);
   }

   ChainShapeAnalyzer::analyze(_world);
}

const sf::Vector2f& Level::getStartPosition() const
{
   return _start_position;
}

Level* Level::getCurrentLevel()
{
   return __current_level;
}

const std::vector<std::shared_ptr<GameMechanism>>& Level::getCheckpoints() const
{
   return _mechanism_checkpoints;
}
