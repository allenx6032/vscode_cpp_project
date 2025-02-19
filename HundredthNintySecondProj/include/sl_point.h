#pragma once

#include "sl_transform.h"

void sliPointInit();
void sliPointDestroy();

void sliPoint(Mat4 *modelview, Vec4 *color);
void sliPointsFlush();
