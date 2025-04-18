// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#ifndef ME_TEXTURES_HPP
#define ME_TEXTURES_HPP

#include "FallingSand_core.hpp"
#include "FallingSand_sdl_wrapper.h"
#include "FallingSand_engine.hpp"
#include "FallingSand_renderer_gpu.h"

namespace ME {

// 贴图类
class Texture {
private:
    C_Surface *m_surface = nullptr;
    R_Image *m_image = nullptr;

public:
    Texture(const std::string &path);
    Texture(C_Surface *sur, bool init_image = true) noexcept;
    ~Texture();

    C_Surface *surface() const { return m_surface; }
    R_Image *image() const { return m_image; }
};

// 贴图引用
using TextureRef = ref<Texture>;

struct TexturePack {
    TextureRef testTexture;
    TextureRef dirt1Texture;
    TextureRef stone1Texture;
    TextureRef smoothStone;
    TextureRef cobbleStone;
    TextureRef flatCobbleStone;
    TextureRef smoothDirt;
    TextureRef cobbleDirt;
    TextureRef flatCobbleDirt;
    TextureRef softDirt;
    TextureRef cloud;
    TextureRef gold;
    TextureRef goldMolten;
    TextureRef goldSolid;
    TextureRef iron;
    TextureRef obsidian;
    TextureRef caveBG;
    TextureRef testAse;

    TextureRef testVacuum;
    TextureRef testHammer;
    TextureRef testPickaxe;
    TextureRef testBucket;
    TextureRef testBucketFilled;
};

void InitTexture(TexturePack &tex);
void EndTexture(TexturePack &tex);

TextureRef LoadTexture(const std::string &path);
TextureRef LoadTextureInternal(const std::string &path, u32 pixelFormat, bool init_image = true);
C_Surface *ScaleSurface(C_Surface *src, f32 x, f32 y);
TextureRef LoadAsepriteTexture(const std::string &path, bool init_image = true);
void RenderTextureRect(TextureRef tex, R_Target *target, int x, int y, MErect *clip = nullptr);

}  // namespace ME

#endif
