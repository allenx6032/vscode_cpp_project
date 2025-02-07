#ifndef UNITY_BUILD
 #pragma once
 #include <stdint.h>
#endif

#define function static
#define internal static

#define Kilobytes(Value) ((Value)*1024)
#define Megabytes(Value) (Kilobytes(Value)*1024)
#define Gigabytes(Value) (Megabytes(Value)*1024)

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;

const int FPS = 30;
const int TICKS_FPS = 1000 / FPS;
const int RENDER_SCALE = 4;

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_W_CAM = SCREEN_WIDTH / 2;
const int SCREEN_H_CAM = 2 * (SCREEN_HEIGHT / 3);

const int CHARACTER_SPEED = 10;

const int BG_R = 128;
const int BG_G = 153;
const int BG_B = 151;

const int ANIMATION_NEXT_UPDATE = 3;
