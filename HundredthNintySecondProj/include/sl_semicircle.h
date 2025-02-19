#pragma once

#include "sl_transform.h"

void sliSemiCircleInit();
void sliSemiCircleDestroy();

void sliSemiCircleOutline(Mat4 *modelview, Vec4 *color, double radius, int numVertices, double degrees);
void sliSemiCircleFill(Mat4 *modelview, Vec4 *color, double radius, int numVertices, double degrees);
