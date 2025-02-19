#pragma once

#include "sl_transform.h"

void sliTextInit();
void sliTextDestroy();

double sliTextWidth(const char *text);
double sliTextHeight(const char *text);
void sliText(Mat4 *modelview, Vec4 *color, const char *text);
void sliTextFlush(Mat4 *modelview, Vec4 *color);

int sliLoadFont(const char *fontFilename);
void sliFont(int font, int fontSize);
void sliFontSize(int fontSize);
