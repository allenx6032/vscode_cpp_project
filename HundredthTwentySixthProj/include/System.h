#ifndef _SYSTEM_H_
#define _SYSTEM_H_
#include "Graphics.h"
#include "Input.h"
#include "FreeCamera.h"
int Start();

int Init();
int Run();

void Frame();
void Exit();


void WindowResizeCallback(GLFWwindow *window,int x,int y);

#endif
