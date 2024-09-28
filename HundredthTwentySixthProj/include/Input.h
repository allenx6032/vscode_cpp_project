#ifndef _INPUT_H_
#define _INPUT_H_
#include "global.h"

void ScrollCallback(GLFWwindow* wnd, double xOffset,double yOffset);
void ScrollProcessing();

void KeyCallback(GLFWwindow * wnd, int key, int scancode,int action,int mods);
void CursorPosCallback(GLFWwindow* wnd,double xPos,double yPos);
void MouseButtonCallback(GLFWwindow * wnd, int button, int action, int mode);
void MouseInputProcessing();
void ResetKeyUp();
void ResetMouseUp();
#endif