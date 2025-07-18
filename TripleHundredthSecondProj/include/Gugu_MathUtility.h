#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu_Types.h"

#include <cmath>
#include <cstdlib>
#include <algorithm>

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {
    
//----------------------------------------------
// Constants

namespace math
{
    const float Pi          = 3.14159265358979323846f;      // Pi
    const float PiDivTwo    = 1.57079632679489661923f;      // Pi / 2
    const float PiDivFour   = 0.785398163397448309616f;     // Pi / 4
    const float OneDivPi    = 0.318309886183790671538f;     // 1 / Pi
    const float TwoDivPi    = 0.636619772367581343076f;     // 2 / Pi

    const float DegreesToRadians = 0.0174532925199f;    // Pi / 180
    const float RadiansToDegrees = 57.2957795131f;      // 180 / Pi

    const float Epsilon3 = 0.001f;
    const float Epsilon6 = 0.000001f;
}

//----------------------------------------------
// General

bool ApproxEqual(float left, float right, float epsilon);
bool ApproxInferior(float left, float right, float epsilon);
bool ApproxSuperior(float left, float right, float epsilon);
bool ApproxInferiorOrEqual(float left, float right, float epsilon);
bool ApproxSuperiorOrEqual(float left, float right, float epsilon);

bool ApproxEqualToZero(float left, float epsilon);
bool ApproxInferiorToZero(float left, float epsilon);
bool ApproxSuperiorToZero(float left, float epsilon);
bool ApproxInferiorOrEqualToZero(float left, float epsilon);
bool ApproxSuperiorOrEqualToZero(float left, float epsilon);

template <typename T>
bool IsInBounds(T value, T min, T max);

template <typename T>
bool ApproxIsInBounds(T value, T min, T max, float epsilon);

template <typename T>
bool IsInBoundsUnordered(T value, T limitA, T limitB);

template <typename T>
bool ApproxIsInBoundsUnordered(T value, T limitA, T limitB, float epsilon);

template <typename T>
T Max(T _tValue, T _tMax);

template <typename T>
T Min(T _tValue, T _tMin);

template <typename T>
T Clamp(T _tValue, T _tMin, T _tMax);

template <typename T>
T ClampUnordered(T _tValue, T _tLimitA, T _tLimitB);

template <typename T>
T Clamp01(T _tValue);

template <typename T>
T Distance(T valueA, T valueB);

template <typename T>
T Absolute(T _tValue);

template <typename T>
T Sign(T value);

template <typename T>
int SignInt(T value);

template <typename T>
T Power(T n, int p);

template <typename T>
T Power2(T n);

float TruncatedModulof(float value, float modulus);  // Result has the same sign as the value.
float FlooredModulof(float value, float modulus);    // Result has the same sign as the modulus.
float EuclideanModulof(float value, float modulus);  // Result is always positive.

template <typename T>
T TruncatedModulo(T value, T modulus);  // Result has the same sign as the value.

template <typename T>
T FlooredModulo(T value, T modulus);    // Result has the same sign as the modulus.

template <typename T>
T EuclideanModulo(T value, T modulus);  // Result is always positive.

//--------------------------------
// Rounding

template <typename T>
int RoundNearestInt(const T& n);

template <typename T>
T RoundNearest(const T& n);

template <typename T>
T RoundFloor(const T& n);

template <typename T>
T RoundCeil(const T& n);

template <typename T>
T RoundTowardZero(const T& n);

template <typename T>
T RoundAwayFromZero(const T& n);

//----------------------------------------------
// Angles

double ToRadians(double _fDegrees);
double ToDegrees(double _fRadians);

float ToRadiansf(float _fDegrees);
float ToDegreesf(float _fRadians);

//----------------------------------------------
// Interpolations

template <typename T>
T Lerp(const T& min, const T& max, float ratio);

template <typename T>
float InverseLerp(const T& min, const T& max, const T& value);

template <typename TFrom, typename TTo>
TTo RemapLerp(const TFrom& minA, const TFrom& maxA, const TTo& minB, const TTo& maxB, const TFrom& value);

// Easing methods from https://github.com/ai/easings.net/blob/master/src/easings/easingsFunctions.ts
float EaseInQuad(float ratio);
float EaseOutQuad(float ratio);
float EaseInOutQuad(float ratio);

float EaseInCubic(float ratio);
float EaseOutCubic(float ratio);
float EaseInOutCubic(float ratio);

float EaseInQuart(float ratio);
float EaseOutQuart(float ratio);
float EaseInOutQuart(float ratio);

float EaseInQuint(float ratio);
float EaseOutQuint(float ratio);
float EaseInOutQuint(float ratio);

float EaseInSine(float ratio);
float EaseOutSine(float ratio);
float EaseInOutSine(float ratio);

float EaseInExpo(float ratio);
float EaseOutExpo(float ratio);
float EaseInOutExpo(float ratio);

float EaseInCirc(float ratio);
float EaseOutCirc(float ratio);
float EaseInOutCirc(float ratio);

float EaseInBack(float ratio);
float EaseOutBack(float ratio);
float EaseInOutBack(float ratio);

float EaseInElastic(float ratio);
float EaseOutElastic(float ratio);
float EaseInOutElastic(float ratio);

float EaseInBounce(float ratio);
float EaseOutBounce(float ratio);
float EaseInOutBounce(float ratio);

}   // namespace gugu

////////////////////////////////////////////////////////////////
// Template Implementation

#include "Gugu_MathUtility.tpp"
