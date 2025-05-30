// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#include "game_ui.hpp"

#include <stdio.h>

#include <string>
#include <thread>

#include "FallingSand_core.hpp"
#include "FallingSand_global.hpp"
#include "FallingSand_filesystem.h"
#include "FallingSand_ecs.hpp"
#include "FallingSand_engine.hpp"
#include "FallingSand_lua_wrapper.hpp"
#include "FallingSand_scripting.hpp"
#include "FallingSand_dbgui.hpp"
#include "FallingSand_imgui_impl.hpp"
#include "FallingSand_ui.hpp"
#include "FallingSand_utility.hpp"
#include "FallingSand_game.hpp"
#include "FallingSand_items.hpp"
#include "FallingSand_player.hpp"
#include "FallingSand_game_datastruct.hpp"
#include "FallingSand_textures.hpp"
#include "FallingSand_world_generator.h"

namespace ME {

#define LANG(_c) global.I18N.Get(_c).c_str()

GameUI::GameUI gameUI;

void I18N::Init() {
    scripting &L = the<scripting>();
    ME_ASSERT(L.s_lua.state(), "Can't load I18N when luacore is invaild");

    Load("zh");
}

void I18N::Load(std::string lang) { the<scripting>().s_lua["setlocale"](lang); }

std::string I18N::Get(std::string text) { return the<scripting>().s_lua["translate"](text); }

namespace GameUI {

void OptionsUI__Draw(game *game) {

    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize("选项").x / 2);
    ImGui::Text("选项");

    static int prevTab = 0;
    int tab = 0;

    if (ImGui::Button("返回")) {
        lua_wrapper::LuaRef s = the<scripting>().s_lua["game_datastruct"]["ui"];
        s["state"] = 0;
    }
    if (ImGui::Button("保存")) {
        // global.game->GameIsolate_.globaldef.Save(METADOT_RESLOC("data/scripts/settings2.lua"));
    }

    ImGui::Separator();

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("OptionsTabs", tab_bar_flags)) {

        if (ImGui::BeginTabItem("全局")) {
            tab = 0;
            ImGui::BeginChild("OptionsTabsCh");

            OptionsUI__DrawGeneral(game);

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("视频")) {
            tab = 1;
            ImGui::BeginChild("OptionsTabsCh");

            OptionsUI__DrawVideo(game);

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("音频")) {
            tab = 2;
            ImGui::BeginChild("OptionsTabsCh");

            OptionsUI__DrawAudio(game);

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("输入")) {
            tab = 3;
            ImGui::BeginChild("OptionsTabsCh");

            OptionsUI__DrawInput(game);

            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    if (tab != prevTab) {
        global.audio.PlayEvent("event:/GUI/GUI_Tab");
        prevTab = tab;
    }
}

void OptionsUI__DrawGeneral(game *game) {
    ImGui::TextColored(ImVec4(1.0, 1.0, 0.8, 1.0), "%s", "游戏内容");
    ImGui::Indent(4);

    ImGui::Checkbox("材质工具提示", &global.game->Iso.globaldef.draw_material_info);

    ImGui::Unindent(4);
}

void OptionsUI__DrawVideo(game *game) {
    ImGui::TextColored(ImVec4(1.0, 1.0, 0.8, 1.0), "%s", "窗口");
    ImGui::Indent(4);

    const char *items[] = {"Windowed", "Fullscreen Borderless", "Fullscreen"};
    const char *combo_label = items[gameUI.OptionsUI__item_current_idx];  // Label to preview before opening the combo (technically it could be anything)

    if (ImGui::BeginCombo("Display Mode", combo_label, 0)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (gameUI.OptionsUI__item_current_idx == n);
            if (ImGui::Selectable(items[n], is_selected)) {

                switch (n) {
                    case 0:
                        ME_win_set_displaymode(E_DisplayMode::WINDOWED);
                        break;
                    case 1:
                        ME_win_set_displaymode(E_DisplayMode::BORDERLESS);
                        break;
                    case 2:
                        ME_win_set_displaymode(E_DisplayMode::FULLSCREEN);
                        break;
                }

                gameUI.OptionsUI__item_current_idx = n;
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (ImGui::Checkbox("VSync", &gameUI.OptionsUI__vsync)) {
        ME_set_vsync(gameUI.OptionsUI__vsync);
    }

    if (ImGui::Checkbox("失去焦点后最小化", &gameUI.OptionsUI__minimizeOnFocus)) {
        ME_win_set_minimize_onlostfocus(gameUI.OptionsUI__minimizeOnFocus);
    }

    ImGui::Unindent(4);
    ImGui::Separator();

    ImGui::TextColored(ImVec4(1.0, 1.0, 0.8, 1.0), "%s", "Rendering");
    ImGui::Indent(4);

    if (ImGui::Checkbox("高清贴图", &global.game->Iso.globaldef.hd_objects)) {
        R_FreeTarget(game->TexturePack_.textureObjects->target);
        R_FreeImage(game->TexturePack_.textureObjects);
        R_FreeTarget(game->TexturePack_.textureObjectsBack->target);
        R_FreeImage(game->TexturePack_.textureObjectsBack);
        R_FreeTarget(game->TexturePack_.textureEntities->target);
        R_FreeImage(game->TexturePack_.textureEntities);

        game->TexturePack_.textureObjects =
                R_CreateImage(game->Iso.world->width * (global.game->Iso.globaldef.hd_objects ? global.game->Iso.globaldef.hd_objects_size : 1),
                              game->Iso.world->height * (global.game->Iso.globaldef.hd_objects ? global.game->Iso.globaldef.hd_objects_size : 1), R_FormatEnum::R_FORMAT_RGBA);
        R_SetImageFilter(game->TexturePack_.textureObjects, R_FILTER_NEAREST);

        game->TexturePack_.textureObjectsBack =
                R_CreateImage(game->Iso.world->width * (global.game->Iso.globaldef.hd_objects ? global.game->Iso.globaldef.hd_objects_size : 1),
                              game->Iso.world->height * (global.game->Iso.globaldef.hd_objects ? global.game->Iso.globaldef.hd_objects_size : 1), R_FormatEnum::R_FORMAT_RGBA);
        R_SetImageFilter(game->TexturePack_.textureObjectsBack, R_FILTER_NEAREST);

        R_LoadTarget(game->TexturePack_.textureObjects);
        R_LoadTarget(game->TexturePack_.textureObjectsBack);

        game->TexturePack_.textureEntities =
                R_CreateImage(game->Iso.world->width * (global.game->Iso.globaldef.hd_objects ? global.game->Iso.globaldef.hd_objects_size : 1),
                              game->Iso.world->height * (global.game->Iso.globaldef.hd_objects ? global.game->Iso.globaldef.hd_objects_size : 1), R_FormatEnum::R_FORMAT_RGBA);
        R_SetImageFilter(game->TexturePack_.textureEntities, R_FILTER_NEAREST);

        R_LoadTarget(game->TexturePack_.textureEntities);
    }

    ImGui::SetNextItemWidth(100);
    ImGui::SliderFloat("光照质量", &global.game->Iso.globaldef.lightingQuality, 0.0, 1.0, "", 0);
    ImGui::Checkbox("简单光照", &global.game->Iso.globaldef.simpleLighting);
    ImGui::Checkbox("抖动光照", &global.game->Iso.globaldef.lightingDithering);

    ImGui::Unindent(4);
}

void OptionsUI__DrawAudio(game *game) {
    ImGui::TextColored(ImVec4(1.0, 1.0, 0.8, 1.0), "%s", "音频");
    ImGui::Indent(4);

    if (gameUI.OptionsUI__busMap.size() == 0) {
        FMOD::Studio::Bus *busses[20];
        int busCt = 0;
        global.audio.GetBank(METADOT_RESLOC("data/assets/audio/fmod/Build/Desktop/Master.bank"))->getBusList(busses, 20, &busCt);

        gameUI.OptionsUI__busMap = {};

        for (int i = 0; i < busCt; i++) {
            FMOD::Studio::Bus *b = busses[i];
            char path[100];
            int ctPath = 0;
            b->getPath(path, 100, &ctPath);

            gameUI.OptionsUI__busMap[std::string(path)] = b;
        }
    }

    std::vector<std::vector<std::string>> disp = {
            {"bus:/Master", "Master"}, {"bus:/Master/Underwater/Music", "Music"}, {"bus:/Master/GUI", "GUI"}, {"bus:/Master/Underwater/Player", "Player"}, {"bus:/Master/Underwater/World", "World"}};

    for (auto &v : disp) {
        f32 volume = 0;
        gameUI.OptionsUI__busMap[v[0]]->getVolume(&volume);
        volume *= 100;
        if (ImGui::SliderFloat(v[1].c_str(), &volume, 0.0f, 100.0f, "%0.0f%%")) {
            volume = std::max(0.0f, std::min(volume, 100.0f));
            gameUI.OptionsUI__busMap[v[0]]->setVolume(volume / 100.0f);
        }
    }

    ImGui::Unindent(4);
}

void OptionsUI__DrawInput(game *game) {}

void MainMenuUI__RefreshWorlds(game *game) {

    gameUI.MainMenuUI__worlds = {};

    for (auto &p : std::filesystem::directory_iterator(METADOT_RESLOC("saves/"))) {
        std::string worldName = p.path().filename().generic_string();

        if (worldName == ".DS_Store") continue;

        WorldMeta meta = WorldMeta::loadWorldMeta(METADOT_RESLOC(std::format("saves/{0}", worldName).c_str()));

        gameUI.MainMenuUI__worlds.push_back(std::make_tuple(worldName, meta));
    }

    auto sortWorlds = [](std::tuple<std::string, WorldMeta> w1, std::tuple<std::string, WorldMeta> w2) {
        time_t c1 = std::get<1>(w1).lastOpenedTime;
        time_t c2 = std::get<1>(w2).lastOpenedTime;
        return (c1 > c2);
    };

    std::sort(gameUI.MainMenuUI__worlds.begin(), gameUI.MainMenuUI__worlds.end(), sortWorlds);
}

void MainMenuUI__Setup() {

    // C_Surface *logoMT = LoadTexture("data/assets/ui/prev_materialtest.png");
    // materialTestWorld = R_CopyImageFromSurface(logoMT);
    // R_SetImageFilter(materialTestWorld, R_FILTER_NEAREST);
    // SDL_FreeSurface(logoMT);

    // C_Surface *logoDef = LoadTexture("data/assets/ui/prev_default.png");
    // defaultWorld = R_CopyImageFromSurface(logoDef);
    // R_SetImageFilter(defaultWorld, R_FILTER_NEAREST);
    // SDL_FreeSurface(logoDef);

    gameUI.MainMenuUI__setup = true;
}

void MainMenuUI__Draw(game *game) {

    ME_ASSERT(game);

    if (!gameUI.visible_mainmenu) return;

    ImGui::SetNextWindowSize(ImVec2(200, 240));
    ImGui::SetNextWindowPos(the<gui>().imgui->NextWindows(dbgui_tag::DBGUI_MAINMENU, ImVec2(100, 100)), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("MainMenu", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    lua_wrapper::LuaRef s = the<scripting>().s_lua["game_datastruct"]["ui"];

    if (s["state"] == 0) {
        MainMenuUI__DrawMainMenu(game);
    } else if (s["state"] == 3) {
        MainMenuUI__DrawCreateWorldUI(game);
    } else if (s["state"] == 4) {
        MainMenuUI__DrawWorldLists(game);
    } else if (s["state"] == 1) {
        OptionsUI__Draw(game);
    } else if (s["state"] == 5) {
        MainMenuUI__DrawInGame(game);
    }

    ImGui::End();
}

void MainMenuUI__DrawMainMenu(game *game) {

    if (!gameUI.MainMenuUI__setup) {
        MainMenuUI__Setup();
    }

    gameUI.MainMenuUI__pos = ImGui::GetWindowPos();

    // ImTextureID texId = (ImTextureID)R_GetTextureHandle(gameUI.MainMenuUI__title);

    // ImVec2 uv_min = ImVec2(0.0f, 0.0f);                // Top-left
    // ImVec2 uv_max = ImVec2(1.0f, 1.0f);                // Lower-right
    // ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // No tint
    // ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

    // ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - gameUI.MainMenuUI__title->w / 2) * 0.5f, ImGui::GetCursorPosY() + 10));
    // ImGui::Image(texId, ImVec2(gameUI.MainMenuUI__title->w / 2, gameUI.MainMenuUI__title->h / 2), uv_min, uv_max, tint_col, border_col);
    // ImGui::TextColored(ImVec4(211.0f, 211.0f, 211.0f, 255.0f), CC("大摆钟送快递"));

    if (ImGui::Button(LANG("ui_play"))) {
        lua_wrapper::LuaRef s = the<scripting>().s_lua["game_datastruct"]["ui"];
        s["state"] = 4;
    }

    if (ImGui::Button(LANG("ui_option"))) {
        lua_wrapper::LuaRef s = the<scripting>().s_lua["game_datastruct"]["ui"];
        s["state"] = 1;
    }

    if (ImGui::Button(LANG("ui_exit"))) {
        game->running = false;
    }
}

void MainMenuUI__DrawInGame(game *game) {

    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize("选项").x / 2);
    ImGui::Text("选项");

    if (ImGui::Button("继续")) {
        gameUI.visible_mainmenu = false;
    }

    if (ImGui::Button("选项")) {
        lua_wrapper::LuaRef s = the<scripting>().s_lua["game_datastruct"]["ui"];
        s["state"] = 1;
    }

    if (ImGui::Button("离开到主菜单")) {
        lua_wrapper::LuaRef s = the<scripting>().s_lua["game_datastruct"]["ui"];
        s["state"] = 0;
        game->quitToMainMenu();
    }
}

void MainMenuUI__DrawCreateWorldUI(game *game) {

    if (!gameUI.MainMenuUI__setup) MainMenuUI__Setup();

    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize(LANG("ui_create_world")).x / 2);
    ImGui::Text("%s", LANG("ui_create_world"));

    ImGui::Text("%s: %s", LANG("ui_worldname"), gameUI.MainMenuUI__worldFolderLabel.c_str());

    const char *world_types[] = {"Material Test World", "Default World (WIP)"};

    ImGui::ListBox(LANG("ui_worldgenerator"), &gameUI.MainMenuUI__selIndex, world_types, IM_ARRAYSIZE(world_types), 4);

    if (ImGui::Button(LANG("ui_return"))) {
        lua_wrapper::LuaRef s = the<scripting>().s_lua["game_datastruct"]["ui"];
        s["state"] = 4;
    }

    if (!gameUI.MainMenuUI__createWorldButtonEnabled) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }

    if (ImGui::Button(LANG("ui_create"))) {

        std::string pref = "Saved in: ";

        std::string worldName = gameUI.MainMenuUI__worldFolderLabel.substr(pref.length());
        char *wn = (char *)worldName.c_str();

        std::string worldTitle = std::string(gameUI.MainMenuUI__worldNameBuf);
        std::regex trimWhitespaceRegex("^ *(.+?) *$");
        worldTitle = regex_replace(worldTitle, trimWhitespaceRegex, "$1");

        METADOT_INFO(std::format("Creating world named \"{0}\" at \"{1}\"", worldTitle, METADOT_RESLOC(std::format("saves/{0}", wn).c_str())).c_str());
        gameUI.visible_mainmenu = false;
        lua_wrapper::LuaRef s = the<scripting>().s_lua["game_datastruct"]["ui"];
        s["state"] = 5;

        game->setGameState(LOADING, INGAME);

        auto w_old = game->Iso.world.release();
        delete w_old;

        WorldGenerator *generator;

        // Delete generator in World::~World()
        if (gameUI.MainMenuUI__selIndex == 0) {
            generator = new MaterialTestGenerator();
        } else if (gameUI.MainMenuUI__selIndex == 1) {
            generator = new DefaultGenerator();
        } else {
            generator = new MaterialTestGenerator();
        }

        std::string wpStr = METADOT_RESLOC(std::format("saves/{0}", wn).c_str());

        game->Iso.world = create_scope<world>();
        game->Iso.world->init(wpStr, (int)ceil(WINDOWS_MAX_WIDTH / 3 / (f64)CHUNK_W) * CHUNK_W + CHUNK_W * 3, (int)ceil(WINDOWS_MAX_HEIGHT / 3 / (f64)CHUNK_H) * CHUNK_H + CHUNK_H * 3,
                              the<engine>().eng()->target, &global.audio, generator);
        game->Iso.world->metadata.worldName = std::string(gameUI.MainMenuUI__worldNameBuf);
        game->Iso.world->metadata.lastOpenedTime = ME_gettime() / 1000;
        game->Iso.world->metadata.lastOpenedVersion = std::to_string(ME_buildnum());
        game->Iso.world->metadata.save(wpStr);

        METADOT_INFO("Queueing chunk loading...");
        for (int x = -CHUNK_W * 4; x < game->Iso.world->width + CHUNK_W * 4; x += CHUNK_W) {
            for (int y = -CHUNK_H * 3; y < game->Iso.world->height + CHUNK_H * 8; y += CHUNK_H) {
                game->Iso.world->queueLoadChunk(x / CHUNK_W, y / CHUNK_H, true, true);
            }
        }
    }

    if (!gameUI.MainMenuUI__createWorldButtonEnabled) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
}

void MainMenuUI__inputChanged(std::string text, game *game) {

    std::regex trimWhitespaceRegex("^ *(.+?) *$");
    text = regex_replace(text, trimWhitespaceRegex, "$1");
    if (text.length() == 0 || text == " ") {
        gameUI.MainMenuUI__worldFolderLabel = "Saved in: ";
        gameUI.MainMenuUI__createWorldButtonEnabled = false;
        return;
    }

    std::regex worldNameInputRegex("^[\\x20-\\x7E]+$");
    gameUI.MainMenuUI__createWorldButtonEnabled = regex_match(text, worldNameInputRegex);

    std::regex worldFolderRegex("[\\/\\\\:*?\"<>|.]");

    std::string worldFolderName = regex_replace(text, worldFolderRegex, "_");
    std::string folder = METADOT_RESLOC(std::format("saves/{0}", worldFolderName).c_str());
    struct stat buffer;
    bool exists = (stat(folder.c_str(), &buffer) == 0);

    std::string newWorldFolderName = worldFolderName;
    int i = 2;
    while (exists) {
        newWorldFolderName = worldFolderName + " (" + std::to_string(i) + ")";
        folder = METADOT_RESLOC(std::format("saves/{0}", newWorldFolderName).c_str());

        exists = (stat(folder.c_str(), &buffer) == 0);

        i++;
    }

    gameUI.MainMenuUI__worldFolderLabel = "Saved in: " + newWorldFolderName;
}

void MainMenuUI__reset(game *game) {
#ifdef _WIN32
    strcpy_s(gameUI.MainMenuUI__worldNameBuf, "New World");
#else
    strcpy(gameUI.MainMenuUI__worldNameBuf, "New World");
#endif
    MainMenuUI__inputChanged(std::string(gameUI.MainMenuUI__worldNameBuf), game);
}

void MainMenuUI__DrawWorldLists(game *game) {
    long long now = ME_gettime();
    if (now - gameUI.MainMenuUI__lastRefresh > 3000) {
        MainMenuUI__RefreshWorlds(game);
        gameUI.MainMenuUI__lastRefresh = now;
    }
    if (!gameUI.visible_mainmenu) return;

    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize(LANG("ui_play")).x / 2);
    ImGui::Text("%s", LANG("ui_play"));

    if (ImGui::Button(LANG("ui_newworld"))) {
        lua_wrapper::LuaRef s = the<scripting>().s_lua["game_datastruct"]["ui"];
        s["state"] = 3;
        MainMenuUI__reset(game);
    }

    if (ImGui::Button(LANG("ui_return"))) {
        lua_wrapper::LuaRef s = the<scripting>().s_lua["game_datastruct"]["ui"];
        s["state"] = 0;
    }

    ImGui::Separator();

    // ImGui::BeginChild("WorldList", ImVec2(0, 200), false);

    int nMainMenuButtons = 0;
    for (auto &t : gameUI.MainMenuUI__worlds) {
        std::string worldName = std::get<0>(t);

        WorldMeta meta = std::get<1>(t);

        ImGui::PushID(nMainMenuButtons);

        struct tm *timeinfo = localtime(&meta.lastOpenedTime);

        if (ImGui::Selectable(std::format("{0}\n{1} ({2}-{3:02}-{4:02} {5:02}:{6:02}:{7:02})", meta.worldName, worldName.c_str(), timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
                                          timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec)
                                      .c_str())) {
            METADOT_INFO("Selected world: ", worldName.c_str());

            gameUI.visible_mainmenu = false;
            lua_wrapper::LuaRef s = the<scripting>().s_lua["game_datastruct"]["ui"];
            s["state"] = 5;

            game->fadeOutStart = the<engine>().eng()->time.now;
            game->fadeOutLength = 250;
            game->fadeOutCallback = [&, game, worldName]() {
                game->setGameState(LOADING, INGAME);

                auto w_old = game->Iso.world.release();
                delete w_old;

                game->Iso.world = create_scope<world>();
                game->Iso.world->init(METADOT_RESLOC(std::format("saves/{0}", worldName).c_str()), (int)ceil(WINDOWS_MAX_WIDTH / 3 / (f64)CHUNK_W) * CHUNK_W + CHUNK_W * 3,
                                      (int)ceil(WINDOWS_MAX_HEIGHT / 3 / (f64)CHUNK_H) * CHUNK_H + CHUNK_H * 3, the<engine>().eng()->target, &global.audio);
                game->Iso.world->metadata.lastOpenedTime = ME_gettime() / 1000;
                game->Iso.world->metadata.lastOpenedVersion = std::to_string(ME_buildnum());
                game->Iso.world->metadata.save(game->Iso.world->worldName);

                METADOT_INFO("Queueing chunk loading...");
                for (int x = -CHUNK_W * 4; x < game->Iso.world->width + CHUNK_W * 4; x += CHUNK_W) {
                    for (int y = -CHUNK_H * 3; y < game->Iso.world->height + CHUNK_H * 8; y += CHUNK_H) {
                        game->Iso.world->queueLoadChunk(x / CHUNK_W, y / CHUNK_H, true, true);
                    }
                }

                game->fadeInStart = the<engine>().eng()->time.now;
                game->fadeInLength = 250;
                game->fadeInWaitFrames = 4;
            };
        }

        ImGui::PopID();
    }
    // ImGui::EndChild();
}

void DebugDrawUI__Setup() {

    gameUI.DebugDrawUI__images = {};
    for (size_t i = 0; i < GAME()->materials_container.size(); i++) {
        Material *mat = GAME()->materials_container[i];
        C_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, 16, 16, 32, SDL_PIXELFORMAT_ARGB8888);
        for (int x = 0; x < surface->w; x++) {
            for (int y = 0; y < surface->h; y++) {
                MaterialInstance m = TilesCreate(mat->id, x, y);
                ME_get_pixel(surface, x, y) = m.color + (m.mat->alpha << 24);
            }
        }
        gameUI.DebugDrawUI__images.push_back(R_CopyImageFromSurface(surface));
        R_SetImageFilter(gameUI.DebugDrawUI__images[i], R_FILTER_NEAREST);
        SDL_FreeSurface(surface);
    }

    gameUI.DebugDrawUI__tools_images = {};
    TextureRef sfc = global.game->Iso.texturepack.testPickaxe;
    gameUI.DebugDrawUI__tools_images.push_back(R_CopyImageFromSurface(sfc->surface()));
    R_SetImageFilter(gameUI.DebugDrawUI__tools_images[0], R_FILTER_NEAREST);
    sfc = global.game->Iso.texturepack.testHammer;
    gameUI.DebugDrawUI__tools_images.push_back(R_CopyImageFromSurface(sfc->surface()));
    R_SetImageFilter(gameUI.DebugDrawUI__tools_images[1], R_FILTER_NEAREST);
    sfc = global.game->Iso.texturepack.testVacuum;
    gameUI.DebugDrawUI__tools_images.push_back(R_CopyImageFromSurface(sfc->surface()));
    R_SetImageFilter(gameUI.DebugDrawUI__tools_images[2], R_FILTER_NEAREST);
    sfc = global.game->Iso.texturepack.testBucket;
    gameUI.DebugDrawUI__tools_images.push_back(R_CopyImageFromSurface(sfc->surface()));
    R_SetImageFilter(gameUI.DebugDrawUI__tools_images[3], R_FILTER_NEAREST);
}

void DebugDrawUI__Draw(game *game) {

    if (gameUI.DebugDrawUI__images.empty()) DebugDrawUI__Setup();

    if (!gameUI.visible_debugdraw) return;

    int width = 5;

    int nRows = ceil(GAME()->materials_container.size() / (f32)width);

    ImGui::SetNextWindowSize(ImVec2(40 * width + 16 + 20, 70 + 5 * 40));
    ImGui::SetNextWindowPos(ImVec2(15, 25), ImGuiCond_FirstUseEver);
    if (global.game->Iso.globaldef.ui_tweak) ImGui::SetNextWindowDockID(the<gui>().imgui->GetMainDockID(), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Debug", NULL, ImGuiWindowFlags_NoResize)) {
        ImGui::End();
        return;
    }

    ImGui::BeginTabBar("ui_debugdraw_tabbar");

    if (ImGui::BeginTabItem(LANG("ui_debug_materials"))) {

        auto a = gameUI.DebugDrawUI__selIndex == -1 ? "None" : gameUI.DebugDrawUI__selectedMaterial->name;
        ImGui::Text("选择: %s", a.data());
        ImGui::Text("放置大小: %d", global.game->Iso.globaldef.brush_size);

        ImGui::Separator();

        ImGui::BeginChild("材料列表", ImVec2(0, 0), false);
        ImGui::Indent(5);
        for (size_t i = 0; i < GAME()->materials_container.size(); i++) {
            int x = (int)(i % width);
            int y = (int)(i / width);

            if (x > 0) ImGui::SameLine();
            ImGui::PushID((int)i);

            ImVec2 selPos = ImGui::GetCursorScreenPos();
            ImGui::SetCursorScreenPos(ImVec2(selPos.x, selPos.y + (x != 0 ? -1 : 0)));
            if (ImGui::Selectable("", gameUI.DebugDrawUI__selIndex == i, 0, ImVec2(32, 36))) {
                gameUI.DebugDrawUI__selIndex = (int)i;
                gameUI.DebugDrawUI__selectedMaterial = GAME()->materials_container[i];
            }

            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("%s", GAME()->materials_container[i]->name.data());
                ImGui::EndTooltip();
            }

            ImVec2 prevPos = ImGui::GetCursorScreenPos();
            ImGuiStyle &style = ImGui::GetStyle();
            ImGui::SetCursorScreenPos(ImVec2(selPos.x - 1, selPos.y + (x == 0 ? 1 : 0)));

            // imgui_impl_opengl3.cpp implements ImTextureID as GLuint
            ImTextureID texId = (ImTextureID)R_GetTextureHandle(gameUI.DebugDrawUI__images[i]);

            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImVec2 uv_min = ImVec2(0.0f, 0.0f);                // Top-left
            ImVec2 uv_max = ImVec2(1.0f, 1.0f);                // Lower-right
            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // No tint
            ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);

            ImGui::Image(texId, ImVec2(32, 32), uv_min, uv_max, tint_col, border_col);

            ImGui::SetCursorScreenPos(prevPos);

            ImGui::PopID();

            /*mn->hoverCallback = [hoverMaterialLabel](Material* mat) {
        hoverMaterialLabel->text = mat->name;
        hoverMaterialLabel->updateTexture();
    };
    mn->selectCallback = [&](Material* mat) {
        selectMaterialLabel->text = mat->name;
        selectMaterialLabel->updateTexture();
        selectedMaterial = mat;
    };*/
        }

        ImGui::Unindent(5);
        ImGui::EndChild();
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem(LANG("ui_debug_items"))) {

        if (ImGui::CollapsingHeader("获得物品")) {
            ImGui::Indent();
            if (game->Iso.world == nullptr || !game->Iso.world->isPlayerInWorld()) {
                ImGui::Text("世界中没有玩家");
            } else {
                int i = 0;
                ImGui::PushID(i);
                int frame_padding = 4;                             // -1 == uses default padding (style.FramePadding)
                ImVec2 size = ImVec2(48, 48);                      // Size of the image we want to make visible
                ImVec2 uv0 = ImVec2(0.0f, 0.0f);                   // UV coordinates for lower-left
                ImVec2 uv1 = ImVec2(1.0f, 1.0f);                   // UV coordinates for (32,32) in our texture
                ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);    // Black background
                ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // No tint

                ImTextureID texId = (ImTextureID)R_GetTextureHandle(gameUI.DebugDrawUI__tools_images[i]);
                if (ImGui::ImageButton(texId, size, uv0, uv1, frame_padding, bg_col, tint_col)) {
                    Item *i3 = new Item();
                    i3->setFlag(ItemFlags::ItemFlags_Tool);
                    i3->texture = global.game->Iso.texturepack.testPickaxe;
                    // i3->image = R_CopyImageFromSurface(i3->surface);
                    // R_SetImageFilter(i3->image, R_FILTER_NEAREST);
                    i3->pivotX = 2;

                    auto pl = game->Iso.world->Reg().find_component<Player>(game->Iso.world->player);
                    pl->setItemInHand(game->Iso.world->Reg().find_component<WorldEntity>(game->Iso.world->player), i3, game->Iso.world.get());
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", "Pickaxe");
                    ImGui::EndTooltip();
                }
                ImGui::PopID();
                ImGui::SameLine();

                i++;

                ImGui::PushID(i);
                texId = (ImTextureID)R_GetTextureHandle(gameUI.DebugDrawUI__tools_images[i]);
                if (ImGui::ImageButton(texId, size, uv0, uv1, frame_padding, bg_col, tint_col)) {
                    Item *i3 = new Item();
                    i3->setFlag(ItemFlags::ItemFlags_Hammer);
                    i3->texture = global.game->Iso.texturepack.testHammer;
                    // i3->image = R_CopyImageFromSurface(i3->surface);
                    // R_SetImageFilter(i3->image, R_FILTER_NEAREST);
                    i3->pivotX = 2;
                    auto pl = game->Iso.world->Reg().find_component<Player>(game->Iso.world->player);
                    pl->setItemInHand(game->Iso.world->Reg().find_component<WorldEntity>(game->Iso.world->player), i3, game->Iso.world.get());
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", "Hammer");
                    ImGui::EndTooltip();
                }
                ImGui::PopID();

                i++;

                ImGui::PushID(i);
                texId = (ImTextureID)R_GetTextureHandle(gameUI.DebugDrawUI__tools_images[i]);
                if (ImGui::ImageButton(texId, size, uv0, uv1, frame_padding, bg_col, tint_col)) {
                    Item *i3 = new Item();
                    i3->setFlag(ItemFlags::ItemFlags_Vacuum);
                    i3->texture = global.game->Iso.texturepack.testVacuum;
                    // i3->image = R_CopyImageFromSurface(i3->surface);
                    // R_SetImageFilter(i3->image, R_FILTER_NEAREST);
                    i3->pivotX = 6;
                    auto pl = game->Iso.world->Reg().find_component<Player>(game->Iso.world->player);
                    pl->setItemInHand(game->Iso.world->Reg().find_component<WorldEntity>(game->Iso.world->player), i3, game->Iso.world.get());
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", "Vacuum");
                    ImGui::EndTooltip();
                }
                ImGui::PopID();
                ImGui::SameLine();

                i++;

                ImGui::PushID(i);
                texId = (ImTextureID)R_GetTextureHandle(gameUI.DebugDrawUI__tools_images[i]);
                if (ImGui::ImageButton(texId, size, uv0, uv1, frame_padding, bg_col, tint_col)) {
                    Item *i3 = new Item();
                    i3->setFlag(ItemFlags::ItemFlags_Fluid_Container);
                    i3->texture = global.game->Iso.texturepack.testBucket;
                    i3->capacity = 100;
                    i3->loadFillTexture(global.game->Iso.texturepack.testBucketFilled->surface());
                    // i3->image = R_CopyImageFromSurface(i3->surface);
                    // R_SetImageFilter(i3->image, R_FILTER_NEAREST);
                    i3->pivotX = 0;
                    auto pl = game->Iso.world->Reg().find_component<Player>(game->Iso.world->player);
                    pl->setItemInHand(game->Iso.world->Reg().find_component<WorldEntity>(game->Iso.world->player), i3, game->Iso.world.get());
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", "Bucket");
                    ImGui::EndTooltip();
                }
                ImGui::PopID();
                ImGui::SameLine();
            }
        }

        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();

    ImGui::End();
}

void DebugDrawUI__End() {
    if (!gameUI.DebugDrawUI__images.empty()) {
        for (auto &i : gameUI.DebugDrawUI__images) R_FreeImage(i);
    }
}

}  // namespace GameUI
}  // namespace ME