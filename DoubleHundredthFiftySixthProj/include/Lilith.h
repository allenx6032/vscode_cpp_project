/*
    This file is a part of Lilith. (https://github.com/yoyoengine/Lilith)
    Copyright (C) 2024-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#ifndef LILITH_H
#define LILITH_H

#include <stdio.h>	// snprintf
#include <stdlib.h>	// malloc

#define _USE_MATH_DEFINES   // msvc edge case
#include <math.h>	        // cos, sin, M_PI

/*
    Edge case fallback because I dont feel
    like debugging CI builds anymore!
*/
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#ifdef _WIN32
	#define LLA_API __declspec(dllexport)
#else
	#define LLA_API
#endif

/*
* Lilith Linear Algebra
* 
* A basic linear algebra library for 2D matrix operations,
* created for use with [yoyoengine](https://github.com/yoyoengine).
* 
* No gaurantees are made for correctness or optimization,
* I'm just a hobbyist. :)
*/

/*
* Types
*/

/*
* 3x3 Matrix
* [row][column]
*/
typedef union mat3_t {
    struct {
        float m00; float m01; float m02;
        float m10; float m11; float m12;
        float m20; float m21; float m22;
    };
    float data[3][3];
    float flat[9];
} mat3_t;

/*
* Generic Vector, will be treated
* as row or column implicitly.
*/
typedef union vec2_t {
    struct {
        float x;
        float y;
    };
    float data[2];
} vec2_t;

/*
* Implementation
*/

/*
* Creates a 0'd out 3x3 matrix
*/
LLA_API mat3_t lla_mat3_zero(void);

/*
* Creates a 3x3 matrix with the given values
*/
LLA_API mat3_t lla_mat3(float a, float b, float c, float d, float e, float f, float g, float h, float i);

/*
* Returns a heap string representation
* of a 3x3 matrix, useful for debugging.
* 
* !!! YOU MUST FREE THE RETURNED POINTER !!!
*/
LLA_API const char * lla_mat3_string(mat3_t mat);

/*
* Returns a heap string representation
* of a 2D vector, useful for debugging.
* 
* !!! YOU MUST FREE THE RETURNED POINTER !!!
*/
LLA_API const char * lla_vec2_string(vec2_t vec);

/*
* Returns a 3x3 identity matrix
*/
LLA_API mat3_t lla_mat3_identity(void);

/*
* Add two 3x3 matrices
*/
LLA_API mat3_t lla_mat3_add(mat3_t a, mat3_t b);

/*
* Subtract two 3x3 matrices
*/
LLA_API mat3_t lla_mat3_sub(mat3_t a, mat3_t b);

/*
* Returns the dot product of two 2D vectors
*/
LLA_API float lla_vec2_dot(vec2_t a, vec2_t b);

/*
* Subtracts two 2D vectors
*/
LLA_API vec2_t lla_vec2_sub(vec2_t a, vec2_t b);

/*
* Adds two 2D vectors
*/
LLA_API vec2_t lla_vec2_add(vec2_t a, vec2_t b);

/*
* Scales a 2D vector by a given scalar
*/
LLA_API vec2_t lla_vec2_scale(vec2_t vec, float scalar);

/*
* Scales a 3x3 matrix by a given scalar
*/
LLA_API mat3_t lla_mat3_scale(mat3_t mat, float scalar);

/*
* Returns the transpose of a 3x3 matrix
*/
LLA_API mat3_t lla_mat3_transpose(mat3_t mat);

/*
* Multiplies two 3x3 matrices
*/
LLA_API mat3_t lla_mat3_mult(mat3_t a, mat3_t b);

/*
* Multiply a 3x3 matrix by a 2D vector
*
* Technically, this is impossible, but we can append a 1 to the vector for our use cases
*/
LLA_API vec2_t lla_mat3_mult_vec2(mat3_t mat, vec2_t vec);

/*
* Translates a 3x3 matrix by a 2D vector
*/
LLA_API mat3_t lla_mat3_translate(mat3_t mat, vec2_t vec);

/*
* Rotates a 3x3 matrix by a given angle in degrees
*/
LLA_API mat3_t lla_mat3_rotate(mat3_t mat, float angle);

/*
* Cross product of two 2D vectors
*/
LLA_API float lla_vec2_cross(vec2_t a, vec2_t b);

/*
* Magnitude of a 2D vector
*/
LLA_API float lla_vec2_magnitude(vec2_t vec);

/*
* Returns the inverse of a 3x3 matrix
*/
LLA_API mat3_t lla_mat3_inverse(mat3_t mat);

/*
* Rotates a 3x3 matrix around a point by a given angle in degrees
*/
LLA_API mat3_t lla_mat3_rotate_around(mat3_t mat, vec2_t point, float angle);

/*
* Scales a 3x3 matrix by a 2D vector (x,y)
*/
LLA_API mat3_t lla_mat3_scale_vec2(mat3_t mat, vec2_t vec);

LLA_API vec2_t lla_vec2_normalize(vec2_t vec);

#endif // LILITH_H
