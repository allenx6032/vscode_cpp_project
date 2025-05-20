/*
    This file is a part of Lilith. (https://github.com/yoyoengine/Lilith)
    Copyright (C) 2024-2025  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

#include <Lilith.h>

mat3_t lla_mat3_zero() {
	mat3_t zero = {
		.data = {
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f}
		}
	};
	return zero;
}

mat3_t lla_mat3(float a, float b, float c, float d, float e, float f, float g, float h, float i) {
	mat3_t mat = {
		.data = {
			{a, b, c},
			{d, e, f},
			{g, h, i}
		}
	};
	return mat;
}

const char * lla_mat3_string(mat3_t mat) {
	// (20 chars per float * 9 floats) + ((2 commas + 2 brackets) * 3 rows) + 32 padding
	unsigned int buffer_size = (20 * 9) + ((2+2)*3) + 32;
	char *buffer = (char*)malloc(buffer_size);
	
	snprintf(buffer, buffer_size, "[%f, %f, %f]\n[%f, %f, %f]\n[%f, %f, %f]", 
		mat.data[0][0], mat.data[0][1], mat.data[0][2],
		mat.data[1][0], mat.data[1][1], mat.data[1][2],
		mat.data[2][0], mat.data[2][1], mat.data[2][2]
	);

	return (const char *)buffer;
}

const char * lla_vec2_string(vec2_t vec) {
	// (20 chars per float * 2 floats) + (1 comma) + 2 brackets + 16 padding
	unsigned int buffer_size = (20 * 2) + 1 + (1+2) + 16;
	char *buffer = (char*)malloc(buffer_size);
	
	snprintf(buffer, buffer_size, "[%f, %f]", vec.data[0], vec.data[1]);

	return (const char*)buffer;
}

mat3_t lla_mat3_identity() {
	mat3_t ident = {
		.data = {
			{1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f}
		}
	};
	return ident;
}

mat3_t lla_mat3_add(mat3_t a, mat3_t b) {
	mat3_t result;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result.data[i][j] = a.data[i][j] + b.data[i][j];
		}
	}
	return result;
}

mat3_t lla_mat3_sub(mat3_t a, mat3_t b) {
	mat3_t result;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result.data[i][j] = a.data[i][j] - b.data[i][j];
		}
	}
	return result;
}

float lla_vec2_dot(vec2_t a, vec2_t b) {
	return (a.data[0] * b.data[0]) + (a.data[1] * b.data[1]);
}

vec2_t lla_vec2_sub(vec2_t a, vec2_t b) {
	vec2_t result;
	result.data[0] = a.data[0] - b.data[0];
	result.data[1] = a.data[1] - b.data[1];
	return result;
}

vec2_t lla_vec2_add(vec2_t a, vec2_t b) {
	vec2_t result;
	result.data[0] = a.data[0] + b.data[0];
	result.data[1] = a.data[1] + b.data[1];
	return result;
}

vec2_t lla_vec2_scale(vec2_t vec, float scalar) {
	vec2_t result;
	result.data[0] = vec.data[0] * scalar;
	result.data[1] = vec.data[1] * scalar;
	return result;
}

mat3_t lla_mat3_scale(mat3_t mat, float scalar) {
	mat3_t result;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result.data[i][j] = mat.data[i][j] * scalar;
		}
	}
	return result;
}

mat3_t lla_mat3_transpose(mat3_t mat) {
	mat3_t result;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result.data[i][j] = mat.data[j][i];
		}
	}
	return result;
}

mat3_t lla_mat3_mult(mat3_t a, mat3_t b) {
	mat3_t result;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result.data[i][j] = (a.data[i][0] * b.data[0][j]) + (a.data[i][1] * b.data[1][j]) + (a.data[i][2] * b.data[2][j]);
		}
	}
	return result;
}

vec2_t lla_mat3_mult_vec2(mat3_t mat, vec2_t vec) {
	vec2_t result;
	result.data[0] = (mat.data[0][0] * vec.data[0]) + (mat.data[0][1] * vec.data[1]) + (mat.data[0][2] * 1.0f);
	result.data[1] = (mat.data[1][0] * vec.data[0]) + (mat.data[1][1] * vec.data[1]) + (mat.data[1][2] * 1.0f);
	return result;
}

mat3_t lla_mat3_translate(mat3_t mat, vec2_t vec) {
	mat3_t result = lla_mat3_identity();
	result.data[0][2] = vec.data[0];
	result.data[1][2] = vec.data[1];
	return lla_mat3_mult(mat, result);
}

mat3_t lla_mat3_rotate(mat3_t mat, float angle) {
	float radians = angle * (M_PI / 180.0f);
	mat3_t result = lla_mat3_identity();
	result.data[0][0] = cosf(radians);
	result.data[0][1] = -sinf(radians);
	result.data[1][0] = sinf(radians);
	result.data[1][1] = cosf(radians);
	return lla_mat3_mult(mat, result);
}

mat3_t lla_mat3_rotate_around(mat3_t mat, vec2_t point, float angle) {
	mat3_t result = lla_mat3_translate(mat, point);
	result = lla_mat3_rotate(result, angle);
	return lla_mat3_translate(result, lla_vec2_scale(point, -1.0f));
}

float lla_vec2_cross(vec2_t a, vec2_t b) {
	return (a.data[0] * b.data[1]) - (a.data[1] * b.data[0]);
}

mat3_t lla_mat3_inverse(mat3_t mat) {
	float det = (mat.data[0][0] * ((mat.data[1][1] * mat.data[2][2]) - (mat.data[1][2] * mat.data[2][1]))) -
				(mat.data[0][1] * ((mat.data[1][0] * mat.data[2][2]) - (mat.data[1][2] * mat.data[2][0]))) +
				(mat.data[0][2] * ((mat.data[1][0] * mat.data[2][1]) - (mat.data[1][1] * mat.data[2][0])));

	if (det == 0.0f) {
		return lla_mat3_zero();
	}

	mat3_t result;
	result.data[0][0] = ((mat.data[1][1] * mat.data[2][2]) - (mat.data[1][2] * mat.data[2][1])) / det;
	result.data[0][1] = ((mat.data[0][2] * mat.data[2][1]) - (mat.data[0][1] * mat.data[2][2])) / det;
	result.data[0][2] = ((mat.data[0][1] * mat.data[1][2]) - (mat.data[0][2] * mat.data[1][1])) / det;
	result.data[1][0] = ((mat.data[1][2] * mat.data[2][0]) - (mat.data[1][0] * mat.data[2][2])) / det;
	result.data[1][1] = ((mat.data[0][0] * mat.data[2][2]) - (mat.data[0][2] * mat.data[2][0])) / det;
	result.data[1][2] = ((mat.data[0][2] * mat.data[1][0]) - (mat.data[0][0] * mat.data[1][2])) / det;
	result.data[2][0] = ((mat.data[1][0] * mat.data[2][1]) - (mat.data[1][1] * mat.data[2][0])) / det;
	result.data[2][1] = ((mat.data[0][1] * mat.data[2][0]) - (mat.data[0][0] * mat.data[2][1])) / det;
	result.data[2][2] = ((mat.data[0][0] * mat.data[1][1]) - (mat.data[0][1] * mat.data[1][0])) / det;

	return result;
}

mat3_t lla_mat3_scale_vec2(mat3_t mat, vec2_t vec) {
	mat3_t result = mat;
	result.data[0][0] *= vec.data[0];
	result.data[1][1] *= vec.data[1];
	return result;
}

float lla_vec2_magnitude(vec2_t vec) {
	return sqrtf((vec.data[0] * vec.data[0]) + (vec.data[1] * vec.data[1]));
}

vec2_t lla_vec2_normalize(vec2_t vec) {
	float mag = lla_vec2_magnitude(vec);
	vec2_t result = vec;
	if (mag > 0.0f) {
        result.data[0] /= mag;
        result.data[1] /= mag;
    }
	return result;
}
