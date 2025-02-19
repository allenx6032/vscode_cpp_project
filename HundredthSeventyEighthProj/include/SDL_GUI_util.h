#pragma once

/**
 * @file
 * useful utilities
 */

namespace SDL_GUI {
namespace util {
/**
 * convert HSV color value to RGB
 * @param h h value in HSV
 * @param s s value in HSV
 * @param v s value in HSV
 * @param[out] r r value in RGB
 * @param[out] g g value in RGB
 * @param[out] b b value in RGB
 */
void HSV_to_RGB(float h, float s, float v, float *r, float *g, float *b);
}}
