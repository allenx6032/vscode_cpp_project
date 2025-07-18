#include "deceptus_levelscript.h"

#include "deceptus_log.h"
#include "deceptus_camerazoom.h"
#include "deceptus_luaconstants.h"
#include "deceptus_luainterface.h"
#include "deceptus_luanode.h"
#include "deceptus_dialogue.h"
#include "deceptus_extra.h"
#include "deceptus_sensorrect.h"
#include "deceptus_player.h"
#include "deceptus_playercontrols.h"
#include "deceptus_weaponsystem.h"
#include "deceptus_savestate.h"
#include "deceptus_bow.h"
#include "deceptus_weaponfactory.h"

#include <mutex>
#include <regex>

namespace
{

std::mutex instance_mutex;
LevelScript* instance = nullptr;

LevelScript* getInstance()
{
   std::lock_guard<std::mutex> lock(instance_mutex);
   return instance;
}

void setInstance(LevelScript* newInstance)
{
   std::lock_guard<std::mutex> lock(instance_mutex);
   instance = newInstance;
}

void resetInstance()
{
   std::lock_guard<std::mutex> lock(instance_mutex);
   instance = nullptr;
}

/**
 * @brief addCollisionRect add a collision rect that fires when the player intersects
 * @param state lua state
 *    param 1: x position relative to where the object has been placed
 *    param 2: y position relative to where the object has been placed
 *    param 3: collision rect width
 *    param 4: collision rect height
 * @return collision rect id
 */
int32_t addCollisionRect(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc != 4)
   {
      return 0;
   }

   const auto x_px = static_cast<int32_t>(lua_tointeger(state, 1));
   const auto y_px = static_cast<int32_t>(lua_tointeger(state, 2));
   const auto w_px = static_cast<int32_t>(lua_tointeger(state, 3));
   const auto h_px = static_cast<int32_t>(lua_tointeger(state, 4));

   const auto rect_id = getInstance()->addCollisionRect({{x_px, y_px}, {w_px, h_px}});
   lua_pushinteger(state, rect_id);
   return 1;
}

/**
 * @brief addSensorRectCallback add a callback when player intersects with a given sensor rect
 * @param state lua state
 *    param 1: identifier of the sensor rect
 * @return error code
 */
int32_t addSensorRectCallback(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc != 1)
   {
      return 0;
   }

   const auto rect_id = lua_tostring(state, 1);
   getInstance()->addSensorRectCallback(rect_id);
   return 0;
}

/**
 * @brief isMechanismVisible check if a given mechanism is visible
 * @param state lua state
 *    param 1: mechanism search pattern
 *    param 2: mechanism group (optional)
 *    return \c true if mechanism is visible
 * @return error code
 */
int32_t isMechanismVisible(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc < 1 || argc > 2)
   {
      return 0;
   }

   const auto search_pattern = lua_tostring(state, 1);

   std::optional<std::string> group;
   if (argc == 2)
   {
      group = lua_tostring(state, 2);
   }

   const auto visible = getInstance()->isMechanismVisible(search_pattern, group);
   lua_pushboolean(state, visible);
   return 1;
}

/**
 * @brief setMechanismVisible set a mechanism node to visible/invisible
 * @param state lua state
 *    param 1: search pattern
 *    param 2: visible flag
 *    param 3: group (optional)
 * @return error code
 */
int32_t setMechanismVisible(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc < 2 || argc > 3)
   {
      return 0;
   }

   const auto search_pattern = lua_tostring(state, 1);
   const auto visible = lua_toboolean(state, 2);

   std::optional<std::string> group;
   if (argc == 3)
   {
      group = lua_tostring(state, 3);
   }

   getInstance()->setMechanismVisible(search_pattern, visible, group);
   return 0;
}

/**
 * @brief isMechanismEnabled check if a given mechanism is enabled
 * @param state lua state
 *    param 1: mechanism search pattern
 *    param 2: mechanism group
 *    return \c true if mechanism is enabled
 * @return error code
 */
int32_t isMechanismEnabled(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc < 1 || argc > 2)
   {
      return 0;
   }

   const auto search_pattern = lua_tostring(state, 1);

   std::optional<std::string> group;
   if (argc == 2)
   {
      group = lua_tostring(state, 2);
   }

   const auto enabled = getInstance()->isMechanismEnabled(search_pattern, group);
   lua_pushboolean(state, enabled);
   return 1;
}

/**
 * @brief setMechanismEnabled set a mechanism node to enabled/disabled
 * @param state lua state
 *    param 1: search pattern
 *    param 2: enabled flag
 * @return error code
 */
int32_t setMechanismEnabled(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc < 2 || argc > 3)
   {
      return 0;
   }

   const auto search_pattern = lua_tostring(state, 1);
   const auto enabled = lua_toboolean(state, 2);

   std::optional<std::string> group;
   if (argc == 3)
   {
      group = lua_tostring(state, 3);
   }

   getInstance()->setMechanismEnabled(search_pattern, enabled, group);
   return 0;
}

// todo: document
/**
 * @brief showDialogue show a dialogue
 * @param state lua state
 *    param 1: search pattern
 * @return error code
 */
int32_t showDialogue(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc != 1)
   {
      return 0;
   }

   const auto search_pattern = lua_tostring(state, 1);

   getInstance()->showDialogue(search_pattern);
   return 0;
}

/**
 * @brief toggle toggle a mechanism
 * @param state lua state
 *    param 1: mechanism name
 *    param 2: group name
 * @return error code
 */
int32_t toggle(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc < 1 || argc > 2)
   {
      return 0;
   }

   const auto search_pattern = lua_tostring(state, 1);

   std::optional<std::string> group;
   if (argc == 2)
   {
      group = lua_tostring(state, 2);
   }

   getInstance()->toggle(search_pattern, group);
   return 0;
}

/**
 * @brief writeLuaNodeProperty write a property of another lua node
 * @param state lua state
 *    param 1: mechanism name
 *    param 2: property key
 *    param 3: property value
 * @return error code
 */
int32_t writeLuaNodeProperty(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc != 3)
   {
      return 0;
   }

   const auto search_pattern = lua_tostring(state, 1);
   const auto key = lua_tostring(state, 2);
   const auto value = lua_tostring(state, 3);

   getInstance()->writeLuaNodeProperty(search_pattern, key, value);
   return 0;
}

/**
 * @brief setLuaNodeVisible write a property of another lua node
 * @param state lua state
 *    param 1: mechanism name
 *    param 2: visible flag
 * @return error code
 */
int32_t setLuaNodeVisible(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc != 2)
   {
      return 0;
   }

   const auto search_pattern = lua_tostring(state, 1);
   const auto visible = lua_toboolean(state, 2);

   getInstance()->setLuaNodeVisible(search_pattern, visible);
   return 0;
}

/**
 * @brief setLuaNodeActive write a property of another lua node
 * @param state lua state
 *    param 1: mechanism name
 *    param 2: active flag
 * @return error code
 */
int32_t setLuaNodeActive(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc != 2)
   {
      return 0;
   }

   const auto search_pattern = lua_tostring(state, 1);
   const auto active = lua_toboolean(state, 2);

   getInstance()->setLuaNodeActive(search_pattern, active);
   return 0;
}

/**
 * @brief addPlayerSkill add a skill to the player
 * @param state lua state
 *    param 1: skill to add
 * @return error code
 */
int32_t addPlayerSkill(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc != 1)
   {
      return 0;
   }

   const auto skill = static_cast<int32_t>(lua_tointeger(state, 1));

   getInstance()->addPlayerSkill(skill);
   return 0;
}

/**
 * @brief addPlayerHealth add health points to the player
 * @param state lua state
 *    param 1: health points to add
 * @return error code
 */
int32_t addPlayerHealth(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc != 1)
   {
      return 0;
   }

   const auto health_points = static_cast<int32_t>(lua_tointeger(state, 1));

   getInstance()->addPlayerHealth(health_points);
   return 0;
}

/**
 * @brief addPlayerHealthMax add health points to the player's max health
 * @param state lua state
 *    param 1: health points to add
 * @return error code
 */
int32_t addPlayerHealthMax(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc != 1)
   {
      return 0;
   }

   const auto health_points = static_cast<int32_t>(lua_tointeger(state, 1));

   getInstance()->addPlayerHealthMax(health_points);
   return 0;
}

/**
 * @brief removePlayerSkill add a skill to the player
 * @param state lua state
 *    param 1: skill to add
 * @return error code
 */
int32_t removePlayerSkill(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc != 1)
   {
      return 0;
   }

   const auto skill = static_cast<int32_t>(lua_tointeger(state, 1));

   getInstance()->removePlayerSkill(skill);
   return 0;
}

/**
 * @brief debug output a debug message to stdout
 * @param state lua state
 *    param 1: debug message
 * @return error code
 */
int32_t debug(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc != 1)
   {
      return 0;
   }

   const auto message = lua_tostring(state, 1);
   Log::Info() << message;

   return 0;
}

/**
 * @brief giveWeaponBow give bow to player
 * @return error code
 */
int32_t giveWeaponBow(lua_State* /*state*/)
{
   getInstance()->giveWeaponBow();
   return 0;
}

/**
 * @brief giveWeaponGun give gun to player
 * @return error code
 */
int32_t giveWeaponGun(lua_State* /*state*/)
{
   getInstance()->giveWeaponGun();
   return 0;
}

/**
 * @brief giveWeaponSword give sword to player
 * @return error code
 */
int32_t giveWeaponSword(lua_State* /*state*/)
{
   getInstance()->giveWeaponSword();
   return 0;
}

/**
 * @brief lockPlayerControls lock the player's controls
 * @param state lua state
 *    param 1: duration in milliseconds for lock
 * @return error code
 */
int32_t lockPlayerControls(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc != 1)
   {
      return 0;
   }

   const auto duration = static_cast<int32_t>(lua_tointeger(state, 1));

   getInstance()->lockPlayerControls(std::chrono::milliseconds{duration});
   return 0;
}

/**
 * @brief setZoomFactor sets the zoom factor for the camera
 * @param state lua state
 *    param 1: zoom factor
 * @return error code
 */
int32_t setZoomFactor(lua_State* state)
{
   const auto argc = lua_gettop(state);
   if (argc != 1)
   {
      return 0;
   }

   const auto zoom_factor = static_cast<float>(lua_tonumber(state, 1));
   getInstance()->setZoomFactor(zoom_factor);

   return 0;
}

[[noreturn]] void error(lua_State* state, const char* /*scope*/ = nullptr)
{
   // the error message is on top of the stack.
   // fetch it, print32_t it and then pop it off the stack.
   std::stringstream os;
   os << lua_tostring(state, -1);

   Log::Error() << os.str();

   lua_pop(state, 1);

   exit(1);
}

}  // namespace

void LevelScript::update(const sf::Time& dt)
{
   // this might be a valid scenario. not every level needs a script to drive its logic.
   if (!_initialized)
   {
      return;
   }

   luaUpdate(dt);

   const auto player_rect = Player::getCurrent()->getPixelRectInt();
   auto id = 0;
   for (const auto& rect : _collision_rects)
   {
      if (player_rect.findIntersection(rect))
      {
         luaPlayerCollidesWithRect(id);
      };

      id++;
   }
}

LevelScript::LevelScript()
{
   // lua is really c style
   setInstance(this);

   // add 'item added' callback
   auto& inventory = SaveState::getCurrent().getPlayerInfo()._inventory;
   _added_callback = [this](const std::string& item) { luaPlayerReceivedItem(item); };
   inventory._added_callbacks.push_back(_added_callback);

   _enabled_observer_reference = GameMechanismObserver::addListener<GameMechanismObserver::EnabledCallback>(
      [this](const std::string& object_id, const std::string& group_id, bool enabled) { luaMechanismEnabled(object_id, group_id, enabled); }
   );

   _event_observer_reference = GameMechanismObserver::addListener<GameMechanismObserver::EventCallback>(
      [this](const std::string& object_id, const std::string& group_id, const std::string& event_name, const LuaVariant& value)
      { luaMechanismEvent(object_id, group_id, event_name, value); }
   );
}

LevelScript::~LevelScript()
{
   resetInstance();

   // remove 'item added' callback
   auto& inventory = SaveState::getCurrent().getPlayerInfo()._inventory;
   inventory.removeAddedCallback(_added_callback);
}

void LevelScript::setup(const std::filesystem::path& path)
{
   _script_name = path.string();

   _lua_state = luaL_newstate();

   // register callbacks
   lua_register(_lua_state, "addCollisionRect", ::addCollisionRect);
   lua_register(_lua_state, "addPlayerSkill", ::addPlayerSkill);
   lua_register(_lua_state, "addPlayerHealth", ::addPlayerHealth);
   lua_register(_lua_state, "addPlayerHealthMax", ::addPlayerHealthMax);
   lua_register(_lua_state, "addSensorRectCallback", ::addSensorRectCallback);
   lua_register(_lua_state, "giveWeaponBow", ::giveWeaponBow);
   lua_register(_lua_state, "giveWeaponGun", ::giveWeaponGun);
   lua_register(_lua_state, "giveWeaponSword", ::giveWeaponSword);
   lua_register(_lua_state, "isMechanismEnabled", ::isMechanismEnabled);
   lua_register(_lua_state, "isMechanismVisible", ::isMechanismVisible);
   lua_register(_lua_state, "lockPlayerControls", ::lockPlayerControls);
   lua_register(_lua_state, "log", ::debug);
   lua_register(_lua_state, "removePlayerSkill", ::removePlayerSkill);
   lua_register(_lua_state, "setLuaNodeActive", ::setLuaNodeActive);
   lua_register(_lua_state, "setLuaNodeVisible", ::setLuaNodeVisible);
   lua_register(_lua_state, "setMechanismEnabled", ::setMechanismEnabled);
   lua_register(_lua_state, "setMechanismVisible", ::setMechanismVisible);
   lua_register(_lua_state, "setZoomFactor", ::setZoomFactor);
   lua_register(_lua_state, "showDialogue", ::showDialogue);
   lua_register(_lua_state, "toggle", ::toggle);
   lua_register(_lua_state, "writeLuaNodeProperty", ::writeLuaNodeProperty);

   // make standard libraries available in the Lua object
   luaL_openlibs(_lua_state);

   // load program
   auto result = luaL_loadfile(_lua_state, _script_name.c_str());
   if (result == LUA_OK)
   {
      // execute program
      result = lua_pcall(_lua_state, 0, LUA_MULTRET, 0);

      if (result != LUA_OK)
      {
         error(_lua_state);
      }
      else
      {
         luaInitialize();

         // register properties
         for (const auto& prop : _properties)
         {
            luaWriteProperty(prop._name, prop._value);
         }
      }
   }
   else
   {
      auto* error_message = lua_tostring(_lua_state, -1);
      Log::Error() << "Failed loading " << _script_name << ": " << (error_message ? error_message : "unknown error");
      lua_pop(_lua_state, 1);
   }
}

/**
 * @brief LevelScript::luaInitialize called to call the initialize function inside the lua script
 * callback name: initialize
 */
void LevelScript::luaInitialize()
{
   lua_getglobal(_lua_state, FUNCTION_INITIALIZE);
   auto result = lua_pcall(_lua_state, 0, 0, 0);

   if (result != LUA_OK)
   {
      error(_lua_state, FUNCTION_INITIALIZE);
   }

   _initialized = true;
}

/**
 * @brief LevelScript::luaUpdate update the lua node
 * @param dt delta time, passed to luanode in seconds
 * callback name: update
 */
void LevelScript::luaUpdate(const sf::Time& dt)
{
   lua_getglobal(_lua_state, FUNCTION_UPDATE);
   lua_pushnumber(_lua_state, dt.asSeconds());

   auto result = lua_pcall(_lua_state, 1, 0, 0);

   if (result != LUA_OK)
   {
      error(_lua_state, FUNCTION_UPDATE);
   }
}

/**
 * @brief LuaNode::luaWriteProperty write a property of the luanode
 * @param key property key
 * @param value property value
 * callback name: writeProperty
 */
void LevelScript::luaWriteProperty(const std::string& key, const std::string& value)
{
   lua_getglobal(_lua_state, FUNCTION_WRITE_PROPERTY);
   if (lua_isfunction(_lua_state, -1))
   {
      lua_pushstring(_lua_state, key.c_str());
      lua_pushstring(_lua_state, value.c_str());

      const auto result = lua_pcall(_lua_state, 2, 0, 0);

      if (result != LUA_OK)
      {
         error(_lua_state, FUNCTION_WRITE_PROPERTY);
      }
   }
}

/**
 * @brief LuaNode::luaPlayerReceivedExtra called when player received an extra
 * @param extra_name name of the extra
 */
void LevelScript::luaPlayerReceivedExtra(const std::string& extra_name)
{
   lua_getglobal(_lua_state, FUNCTION_PLAYER_RECEIVED_EXTRA);
   if (lua_isfunction(_lua_state, -1))
   {
      lua_pushstring(_lua_state, extra_name.c_str());

      const auto result = lua_pcall(_lua_state, 1, 0, 0);

      if (result != LUA_OK)
      {
         error(_lua_state, FUNCTION_PLAYER_RECEIVED_EXTRA);
      }
   }
}

///
/// \brief LevelScript::luaPlayerReceivedItem
/// \param item item that was received
///
void LevelScript::luaPlayerReceivedItem(const std::string& item)
{
   lua_getglobal(_lua_state, FUNCTION_PLAYER_RECEIVED_ITEM);
   if (lua_isfunction(_lua_state, -1))
   {
      lua_pushstring(_lua_state, item.c_str());

      const auto result = lua_pcall(_lua_state, 1, 0, 0);

      if (result != LUA_OK)
      {
         error(_lua_state, FUNCTION_PLAYER_RECEIVED_ITEM);
      }
   }
}

///
/// \brief LevelScript::luaMechanismEnabled
/// \param object_id object identifier
/// \param group object group
/// \param enabled mechanism enabled flag
///
void LevelScript::luaMechanismEnabled(const std::string& object_id, const std::string& group, bool enabled)
{
   if (_lua_state == nullptr)
   {
      return;
   }

   lua_getglobal(_lua_state, FUNCTION_MECHANISM_ENABLED);
   if (lua_isfunction(_lua_state, -1))
   {
      lua_pushstring(_lua_state, object_id.c_str());
      lua_pushstring(_lua_state, group.c_str());
      lua_pushboolean(_lua_state, enabled);

      const auto result = lua_pcall(_lua_state, 3, 0, 0);

      if (result != LUA_OK)
      {
         error(_lua_state, FUNCTION_MECHANISM_ENABLED);
      }
   }
}

///
/// \brief LevelScript::luaMechanismEvent
/// \param object_id object identifier
/// \param group object group
/// \param event_name event name
/// \param value value that's part of the event
///
void LevelScript::luaMechanismEvent(
   const std::string& object_id,
   const std::string& group,
   const std::string& event_name,
   const LuaVariant& value
)
{
   if (_lua_state == nullptr)
   {
      return;
   }

   lua_getglobal(_lua_state, FUNCTION_MECHANISM_EVENT);
   if (lua_isfunction(_lua_state, -1))
   {
      lua_pushstring(_lua_state, object_id.c_str());
      lua_pushstring(_lua_state, group.c_str());
      lua_pushstring(_lua_state, event_name.c_str());

      std::visit(
         [this](auto&& arg)
         {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>)
            {
               lua_pushstring(_lua_state, arg.c_str());
            }
            else if constexpr (std::is_same_v<T, int64_t>)
            {
               lua_pushinteger(_lua_state, static_cast<lua_Integer>(arg));
            }
            else if constexpr (std::is_same_v<T, double>)
            {
               lua_pushnumber(_lua_state, static_cast<lua_Number>(arg));
            }
            else if constexpr (std::is_same_v<T, bool>)
            {
               lua_pushboolean(_lua_state, arg);
            }
         },
         value
      );

      const auto result = lua_pcall(_lua_state, 4, 0, 0);

      if (result != LUA_OK)
      {
         error(_lua_state, FUNCTION_WRITE_PROPERTY);
      }
   }
}

void LevelScript::setSearchMechanismCallback(const SearchMechanismCallback& callback)
{
   _search_mechanism_callback = callback;
}

void LevelScript::createExtraCallbacks(const std::vector<std::shared_ptr<GameMechanism>>& extras)
{
   // handshake between extra mechanism and level script
   for (const auto& extra_mechanism : extras)
   {
      auto extra = std::dynamic_pointer_cast<Extra>(extra_mechanism);
      extra->_callbacks.emplace_back([this](const std::string& extra) { luaPlayerReceivedExtra(extra); });
   }
}

int32_t LevelScript::addCollisionRect(const sf::IntRect& rect)
{
   _collision_rects.push_back(rect);
   return static_cast<int32_t>(_collision_rects.size());
}

void LevelScript::setMechanismEnabled(const std::string& search_pattern, bool enabled, const std::optional<std::string>& group)
{
   if (!_search_mechanism_callback)
   {
      Log::Error() << "search mechanism callback not initialized yet";
      return;
   }

   auto mechanisms = _search_mechanism_callback(search_pattern, group);
   for (auto& mechanism : mechanisms)
   {
      mechanism->setEnabled(enabled);
   }
}

bool LevelScript::isMechanismEnabled(const std::string& search_pattern, const std::optional<std::string>& group) const
{
   if (!_search_mechanism_callback)
   {
      Log::Error() << "search mechanism callback not initialized yet";
      return false;
   }

   auto mechanisms = _search_mechanism_callback(search_pattern, group);
   if (mechanisms.empty())
   {
      return false;
   }
   return mechanisms.front()->isEnabled();
}

void LevelScript::setMechanismVisible(const std::string& search_pattern, bool visible, const std::optional<std::string>& group)
{
   if (!_search_mechanism_callback)
   {
      Log::Error() << "search mechanism callback not initialized yet";
      return;
   }

   auto mechanisms = _search_mechanism_callback(search_pattern, group);
   for (auto& mechanism : mechanisms)
   {
      mechanism->setVisible(visible);
   }
}

bool LevelScript::isMechanismVisible(const std::string& search_pattern, const std::optional<std::string>& group) const
{
   if (!_search_mechanism_callback)
   {
      Log::Error() << "search mechanism callback not initialized yet";
      return false;
   }

   auto mechanisms = _search_mechanism_callback(search_pattern, group);
   if (mechanisms.empty())
   {
      return false;
   }
   return mechanisms.front()->isVisible();
}

void LevelScript::toggle(const std::string& search_pattern, const std::optional<std::string>& group)
{
   auto mechanisms = _search_mechanism_callback(search_pattern, group);
   for (auto& mechanism : mechanisms)
   {
      mechanism->toggle();
   }
}

void LevelScript::addPlayerSkill(int32_t skill)
{
   SaveState::getPlayerInfo()._extra_table._skills._skills |= skill;
}

void LevelScript::removePlayerSkill(int32_t skill)
{
   SaveState::getPlayerInfo()._extra_table._skills._skills &= ~skill;
}

void LevelScript::addPlayerHealth(int32_t health_points_to_add)
{
   SaveState::getPlayerInfo()._extra_table._health.addHealth(health_points_to_add);
}

void LevelScript::addPlayerHealthMax(int32_t health_points_to_add)
{
   SaveState::getPlayerInfo()._extra_table._health._health_max += health_points_to_add;
}

void LevelScript::setZoomFactor(float zoom_factor)
{
   CameraZoom::getInstance().setZoomFactor(zoom_factor);
}

namespace
{
void giveWeaponToPlayer(const std::shared_ptr<Weapon>& weapon)
{
   auto& weapon_system = SaveState::getPlayerInfo()._weapons;
   weapon_system._weapons.push_back(weapon);
   weapon_system._selected = weapon;
}
}  // namespace

void LevelScript::giveWeaponBow()
{
   auto bow = WeaponFactory::create(WeaponType::Bow);
   std::dynamic_pointer_cast<Bow>(bow)->setLauncherBody(Player::getCurrent()->getBody());
   giveWeaponToPlayer(bow);
}

void LevelScript::giveWeaponGun()
{
   auto gun = WeaponFactory::create(WeaponType::Gun);
   giveWeaponToPlayer(gun);
}

void LevelScript::giveWeaponSword()
{
   auto sword = WeaponFactory::create(WeaponType::Sword);
   giveWeaponToPlayer(sword);
}

std::vector<std::shared_ptr<LuaNode>> LevelScript::findLuaNodes(const std::string& search_pattern)
{
   std::vector<std::shared_ptr<LuaNode>> results;

   const auto& object_list = LuaInterface::instance().getObjectList();
   std::regex pattern(search_pattern);
   for (auto& node : object_list)
   {
      auto lua_node = std::dynamic_pointer_cast<LuaNode>(node);
      if (std::regex_match(lua_node->_name, pattern))
      {
         results.push_back(lua_node);
      }
   }

   return results;
}

void LevelScript::writeLuaNodeProperty(const std::string& search_pattern, const std::string& key, const std::string& value)
{
   const auto results = findLuaNodes(search_pattern);

   if (results.empty())
   {
      Log::Error() << "search pattern " << search_pattern << " did not give any results";
      return;
   }

   for (auto& lua_node : results)
   {
      lua_node->luaWriteProperty(key, value);
   }
}

void LevelScript::setLuaNodeVisible(const std::string& search_pattern, bool visible)
{
   const auto results = findLuaNodes(search_pattern);

   if (results.empty())
   {
      Log::Error() << "search pattern " << search_pattern << " did not give any results";
      return;
   }

   for (auto& lua_node : results)
   {
      lua_node->_visible = visible;
   }
}

void LevelScript::setLuaNodeActive(const std::string& search_pattern, bool active)
{
   const auto results = findLuaNodes(search_pattern);

   if (results.empty())
   {
      Log::Error() << "search pattern " << search_pattern << " did not give any results";
      return;
   }

   for (auto& lua_node : results)
   {
      lua_node->setActive(active);
   }
}

void LevelScript::showDialogue(const std::string& search_pattern)
{
   if (!_search_mechanism_callback)
   {
      Log::Error() << "search mechanism callback not initialized yet";
      return;
   }

   auto mechanisms = _search_mechanism_callback(search_pattern, "dialogues");

   if (mechanisms.empty())
   {
      Log::Error() << "search pattern " << search_pattern << " did not give any results";
      return;
   }

   auto dialogue = std::dynamic_pointer_cast<Dialogue>(mechanisms.front());
   dialogue->setActive(true);
   dialogue->showNext();
}

void LevelScript::lockPlayerControls(const std::chrono::milliseconds& duration)
{
   Player::getCurrent()->getControls()->lockAll(PlayerControls::LockedState::Released, duration);
}

void LevelScript::addSensorRectCallback(const std::string& search_pattern)
{
   if (!_search_mechanism_callback)
   {
      Log::Error() << "search mechanism callback not initialized yet";
      return;
   }

   auto mechanisms = _search_mechanism_callback(search_pattern, "sensor_rects");

   if (mechanisms.empty())
   {
      Log::Error() << "search pattern " << search_pattern << " did not give any results";
      return;
   }

   for (auto& mechanism : mechanisms)
   {
      auto sensor_rect = std::dynamic_pointer_cast<SensorRect>(mechanism);
      if (sensor_rect)
      {
         sensor_rect->addSensorCallback([this](const std::string& rect_id) { luaPlayerCollidesWithSensorRect(rect_id); });
      }
   }
}

void LevelScript::luaPlayerCollidesWithRect(int32_t rect_id)
{
   lua_getglobal(_lua_state, FUNCTION_PLAYER_COLLIDES_WITH_RECT);
   lua_pushnumber(_lua_state, rect_id);

   auto result = lua_pcall(_lua_state, 1, 0, 0);

   if (result != LUA_OK)
   {
      error(_lua_state, FUNCTION_PLAYER_COLLIDES_WITH_RECT);
   }
}

void LevelScript::luaPlayerCollidesWithSensorRect(const std::string& sensor_rect_id)
{
   lua_getglobal(_lua_state, FUNCTION_PLAYER_COLLIDES_WITH_SENSOR_RECT);
   lua_pushstring(_lua_state, sensor_rect_id.c_str());

   auto result = lua_pcall(_lua_state, 1, 0, 0);

   if (result != LUA_OK)
   {
      error(_lua_state, FUNCTION_PLAYER_COLLIDES_WITH_SENSOR_RECT);
   }
}
