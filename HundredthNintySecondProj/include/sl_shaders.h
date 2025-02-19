#pragma once

#include "sl_transform.h"
#include "sl_shader.h"

extern Shader *sliBasicShader;
extern Shader *sliPointShader;
extern Shader *sliTextureShader;
extern Shader *sliTextShader;

void sliShadersInit(Mat4 *projection);
void sliShadersDestroy();
