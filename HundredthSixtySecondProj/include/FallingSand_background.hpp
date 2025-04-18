// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#ifndef ME_BACKGROUND_HPP
#define ME_BACKGROUND_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "FallingSand_core.hpp"
#include "FallingSand_sdl_wrapper.h"
#include "FallingSand_renderer_gpu.h"
#include "FallingSand_lua_wrapper.hpp"
#include "FallingSand_game_datastruct.hpp"
#include "FallingSand_textures.hpp"

namespace ME {

class BackgroundLayer {
public:
    TextureRef tex;

    C_Surface *surface[3];
    R_Image *texture[3];
    f32 parralaxX;
    f32 parralaxY;
    f32 moveX;
    f32 moveY;

public:
    BackgroundLayer(TextureRef tex, f32 parralaxX, f32 parralaxY, f32 moveX, f32 moveY) noexcept;
    ~BackgroundLayer();
};

using BackgroundLayerRef = ref<BackgroundLayer>;

class BackgroundObject {
public:
    u32 solid;
    std::vector<BackgroundLayerRef> layers;
    BackgroundObject(u32 solid, std::vector<BackgroundLayerRef> layers) : solid(std::move(solid)), layers(std::move(layers)){};
    // BackgroundObject(u32 solid, std::vector<BackgroundLayer> layers) : solid(std::move(solid)) {
    //     for (auto layer : layers) {
    //         this->layers.push_back(ME::create_ref<BackgroundLayer>(layer));
    //     }
    // };
    //  BackgroundObject(BackgroundObject &) = default;
    ~BackgroundObject();
    void Init();
};

class BackgroundSystem : public IGameSystem {
public:
    std::unordered_map<std::string, BackgroundObject *> m_backgrounds;

    // static BackgroundObject TEST_OVERWORLD;

    void Push(std::string name, BackgroundObject *bg);
    BackgroundObject *Get(std::string name);

    REGISTER_SYSTEM(BackgroundSystem)

    void create() override;
    void destory() override;
    void reload() override;
    void registerLua(lua_wrapper::State &s_lua) override;

public:
    void draw();
};

}  // namespace ME

#endif
