// Copyright(c) 2022-2023, KaoruXun All rights reserved.

#ifndef ME_GAMESHADERS_HPP
#define ME_GAMESHADERS_HPP

#include "FallingSand_core.hpp"
#include "FallingSand_shaders.hpp"
#include "FallingSand_game_datastruct.hpp"

namespace ME {

class CrtShader : public shader_base {
public:
    bool enable;

    void Update(int w, int h);

    ShaderBaseDecl();
};

class WaterFlowPassShader : public shader_base {
public:
    bool dirty;

    void Update(int w, int h);

    ShaderBaseDecl();
};

class WaterShader : public shader_base {
public:
    void Update(f32 t, int w, int h, R_Image *maskImg, int mask_x, int mask_y, int mask_w, int mask_h, int scale, R_Image *flowImg, int overlay, bool showFlow, bool pixelated);

    ShaderBaseDecl();
};

class NewLightingShader : public shader_base {
public:
    f32 lastLx;
    f32 lastLy;
    f32 lastQuality;
    f32 lastInside;
    bool lastSimpleMode;
    bool lastEmissionEnabled;
    bool lastDitheringEnabled;

    f32 insideDes;
    f32 insideCur;

    void SetSimpleMode(bool simpleMode);
    void SetEmissionEnabled(bool emissionEnabled);
    void SetDitheringEnabled(bool ditheringEnabled);
    void SetQuality(f32 quality);
    void SetInside(f32 inside);
    void SetBounds(f32 minX, f32 minY, f32 maxX, f32 maxY);

    void Update(R_Image *tex, R_Image *emit, f32 x, f32 y);

    ShaderBaseDecl();
};

class FireShader : public shader_base {
public:
    void Update(R_Image *tex);

    ShaderBaseDecl();
};

class Fire2Shader : public shader_base {
public:
    void Update(R_Image *tex);

    ShaderBaseDecl();
};

class BlurShader : public shader_base {
public:
    void Update(R_Image *tex);

    ShaderBaseDecl();
};

class UntexturedShader : public shader_base {
public:
    GLuint VBO;
    GLuint modelViewProjection_loc;
    GLuint vertex_loc;
    GLuint color_loc;
    void Update(float mvp[], GLfloat gldata[]);

    ShaderBaseDecl();
};

class RayLightingShader : public shader_base {
public:

    void Update(R_Image *img, float x, float y);

    ShaderBaseDecl();
};

class shader_worker : public IGameSystem {
public:
    CrtShader *crtShader = nullptr;
    WaterShader *waterShader = nullptr;
    WaterFlowPassShader *waterFlowPassShader = nullptr;
    NewLightingShader *newLightingShader = nullptr;
    FireShader *fireShader = nullptr;
    Fire2Shader *fire2Shader = nullptr;
    BlurShader *blurShader = nullptr;
    UntexturedShader *untexturedShader = nullptr;
    RayLightingShader *raylightingShader = nullptr;

    REGISTER_SYSTEM(shader_worker)

    void create() override;
    void destory() override;
    void reload() override;
    void registerLua(lua_wrapper::State &s_lua) override;
};

}  // namespace ME

#endif