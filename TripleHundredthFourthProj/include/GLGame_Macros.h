// This file contains all the macros required by the ENGINE. 

#define GLGAME_APPLICATION
#define GLGAME 
#define GLGAME_DEBUG
#define GLGAME_VERSION "0.0.1"

#define GLGAME_DEFAULT_BGSHADER_VERTEX		".\\Shaders\\DefaultVertexShader.glsl"
#define GLGAME_DEFAULT_BGSHADER_FRAGMENT	".\\Shaders\\DefaultFragmentShader.glsl"
#define GLGAME_DEFAULT_SE_VERTEX			".\\Shaders\\DefaultVertexShader.glsl"
#define GLGAME_DEFAULT_SE_FRAGMENT			".\\Shaders\\DefaultFragmentShader.glsl"
#define GLGAME_DEFAULT_OBJECT_VERTEX		".\\Shaders\\DefaultVertexShader.glsl"
#define GLGAME_DEFAULT_OBJECT_FRAGMENT		".\\Shaders\\DefaultFragmentShader.glsl"
#define GLGAME_DEFAULT_BATCH_VERTEX			".\\Shaders\\DefaultBatchVertexShader.glsl"
#define GLGAME_DEFAULT_BATCH_FRAGMENT		".\\Shaders\\DefaultBatchFragShader.glsl"
#define GLGAME_DEFAULT_LIGHT_VERTEX			".\\Shaders\\DefaultLightingVertexShader.glsl"
#define GLGAME_DEFAULT_LIGHT_FRAGMENT		".\\Shaders\\DefaultLightingFragShader.glsl"
#define GLGAME_DEFAULT_PARTICLE_VERTEX		".\\Shaders\\DefaultParticleVertexShader.glsl"
#define GLGAME_DEFAULT_PARTICLE_FRAGMENT	".\\Shaders\\DefaultParticleFragmentShader.glsl"

#define GLGAME_RESIZABLE	true

// Not really used in the game. The amount of layers you can use are unlimited. (I used an std::map)
#define GLGAME_MAX_LAYERS	9

#define GLGAME_INT_GLFW_KEY_COUNT 1024

// OpenGL Version 
#define GLGAME_OPENGL_VERSION_MAJOR 3
#define GLGAME_OPENGL_VERSION_MINOR 2

#define GLGAME_DEFAULT_WINDOW_SIZE_W 800
#define GLGAME_DEFAULT_WINDOW_SIZE_H 600

#define GLGAME_SCENE_FILE_HEADER "%GLGAME_SCENE_FILE_V01%"