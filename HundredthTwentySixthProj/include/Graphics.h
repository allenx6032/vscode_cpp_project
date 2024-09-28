#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_
#include <GLew/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "FileLoader.h"
#include "global.h"
#include <time.h>
int InitWindow(int height, int width,char * title);
int InitOpenGL();

int SetupModels();
int SetupMaterials();

int LoadModel(const char* filename, Model* model);
int setupShader(Material* material);
int setupTexture(const char* filename, uint* texture);
void Render();

void Cleanup();

void LoadTextureShaderData(Material *self);
void LoadShaderData(Material *self);
void LoadShaderDataEmpty(Material *self);

void LightMovement();

uint32_t getTick();

#endif 
