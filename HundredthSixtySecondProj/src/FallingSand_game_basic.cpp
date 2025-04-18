// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#include "FallingSand_game_basic.hpp"

#include <string>
#include <string_view>

#include "FallingSand_base_memory.h"
#include "FallingSand_core.hpp"
#include "FallingSand_global.hpp"
#include "FallingSand_filesystem.h"
#include "FallingSand_lua_wrapper.hpp"
#include "FallingSand_scripting.hpp"
#include "FallingSand_game.hpp"
#include "FallingSand_player.hpp"
#include "FallingSand_game_datastruct.hpp"
#include "FallingSand_game_ui.hpp"
#include "FallingSand_reflectionflat.hpp"
#include "FallingSand_textures.hpp"

namespace ME {

#pragma region GameScriptingBind_1

static void audio_init() { global.audio.Init(); }

static void audio_load_bank(std::string name, unsigned int type) { global.audio.LoadBank(METADOT_RESLOC(name.c_str()), type); }

static void audio_load_event(std::string event) { global.audio.LoadEvent(event); }
static void audio_play_event(std::string event) { global.audio.PlayEvent(event); }

static void textures_init() {
    // 贴图初始化
    InitTexture(global.game->Iso.texturepack);
}
static void textures_end() { EndTexture(global.game->Iso.texturepack); }
static void textures_load(std::string name, std::string path) {}
static void controls_init() { input::InitKey(); }

static void init_ecs() { auto &luawrap = the<scripting>().s_lua; }

static void load_lua(std::string luafile) {}

#pragma endregion GameScriptingBind_1

Biome Biome::biomeGet(std::string name) {
    for (auto t : GAME()->biome_container)
        if (t.first == name) return t.second;
    return GAME()->biome_container[0];  // 没有找到指定生物群系则返回默认生物群系
}

void Biome::createBiome(std::string name, int id) {
    METADOT_BUG("[LUA] create_biome ", name, " = ", id);
    // Biome *b = alloc<Biome>::safe_malloc(name, id);
    GAME()->biome_container.insert(std::make_pair(name, Biome{id}));
}

void gameplay::create() {
    METADOT_BUG("GameplayScriptSystem created");

    the<scripting>().fast_load_lua(METADOT_RESLOC("data/scripts/game.lua"));
    the<scripting>().fast_call_func("OnGameEngineLoad")();
    the<scripting>().fast_call_func("OnGameLoad")(global.game);

    // GlobalDEF table initialization
    InitGlobalDEF(&global.game->Iso.globaldef, false);

    // I18N must be initialized after scripting system
    // It uses i18n.lua to function
    global.I18N.Init();
}

void gameplay::destory() { the<scripting>().fast_call_func("OnGameEngineUnLoad")(); }

void gameplay::reload() {}

void gameplay::registerLua(lua_wrapper::State &s_lua) {
    s_lua["controls_init"] = lua_wrapper::function(controls_init);
    s_lua["materials_init"] = lua_wrapper::function(InitMaterials);
    s_lua["materials_register"] = lua_wrapper::function(RegisterMaterial);
    s_lua["materials_push"] = lua_wrapper::function(PushMaterials);
    s_lua["textures_load"] = lua_wrapper::function(textures_load);
    s_lua["textures_init"] = lua_wrapper::function(textures_init);
    s_lua["textures_end"] = lua_wrapper::function(textures_end);
    s_lua["audio_load_event"] = lua_wrapper::function(audio_load_event);
    s_lua["audio_play_event"] = lua_wrapper::function(audio_play_event);
    s_lua["audio_load_bank"] = lua_wrapper::function(audio_load_bank);
    s_lua["audio_init"] = lua_wrapper::function(audio_init);
    s_lua["create_biome"] = lua_wrapper::function(Biome::createBiome);
    s_lua["init_ecs"] = lua_wrapper::function(init_ecs);

    s_lua["DrawMainMenuUI"] = lua_wrapper::function(GameUI::MainMenuUI__Draw);
    s_lua["DrawDebugUI"] = lua_wrapper::function(GameUI::DebugDrawUI__Draw);
    s_lua["DebugUIEnd"] = lua_wrapper::function(GameUI::DebugDrawUI__End);

    // ItemBinding::register_class(s_lua.state());
    RigidBodyBinding::register_class(s_lua.state());

    // Test
    s_lua(R"(
a = RigidBody(1, "hello")
    )");

    {
        lua_getglobal(s_lua.state(), "a");
        auto b = RigidBodyBinding::fromStackThrow(s_lua.state(), -1);
        lua_pop(s_lua.state(), 1);

        METADOT_INFO("Use count is now: ", b.use_count());
    }

    // {
    //     MyActorPtr actor = std::make_shared<MyActor>("Nigel", 39);
    //     METADOT_INFO("Actor use count is: %ld", actor.use_count());
    //     MyActorBinding::push(s_lua.state(), actor);
    //     lua_setglobal(s_lua.state(), "actor");
    //     METADOT_INFO("Pushed to Lua");
    //     METADOT_INFO("Actor use count is: %ld", actor.use_count());
    //     s_lua("actor:walk()");
    //     s_lua("actor.age = actor.age + 1 print( 'Happy Birthday')");
    //     s_lua("print( actor.age )");
    //     METADOT_INFO("%d", actor->_age);
    //     // Should print Coffee, nil as 'added' members/properties are per instance.
    //     s_lua("print( a.extra, actor.extra )");
    // }
}

}  // namespace ME
