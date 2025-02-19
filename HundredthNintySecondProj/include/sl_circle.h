#pragma once

#include "sl_transform.h"

void sliCircleInit();
void sliCircleDestroy();

void sliCircleOutline(Mat4 *modelview, Vec4 *color, double radius, int numVertices);
void sliCircleFill(Mat4 *modelview, Vec4 *color, double radius, int numVertices);
