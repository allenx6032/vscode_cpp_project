#ifndef RAYFORK_MATHS_H
#define RAYFORK_MATHS_H

#include "rayfork/foundation/basicdef.h"
#include "rayfork/math/math-types.h"

rf_extern float rf_next_pot(float it);
rf_extern rf_vec2 rf_center_to_object(rf_sizef center_this, rf_rec to_this); // Returns the position of an object such that it will be centered to a rectangle
rf_extern float rf_clamp(float value, float min, float max); // Clamp float value
rf_extern float rf_lerp(float start, float end, float amount); // Calculate linear interpolation between two floats

rf_extern rf_vec2 rf_vec2_add(rf_vec2 v1, rf_vec2 v2); // Add two vectors (v1 + v2)
rf_extern rf_vec2 rf_vec2_sub(rf_vec2 v1, rf_vec2 v2); // Subtract two vectors (v1 - v2)
rf_extern float rf_vec2_len(rf_vec2 v); // Calculate vector length
rf_extern float rf_vec2_dot_product(rf_vec2 v1, rf_vec2 v2); // Calculate two vectors dot product
rf_extern float rf_vec2_distance(rf_vec2 v1, rf_vec2 v2); // Calculate distance between two vectors
rf_extern float rf_vec2_angle(rf_vec2 v1, rf_vec2 v2); // Calculate angle from two vectors in X-axis
rf_extern rf_vec2 rf_vec2_scale(rf_vec2 v, float scale); // Scale vector (multiply by value)
rf_extern rf_vec2 rf_vec2_mul_v(rf_vec2 v1, rf_vec2 v2); // Multiply vector by vector
rf_extern rf_vec2 rf_vec2_negate(rf_vec2 v); // Negate vector
rf_extern rf_vec2 rf_vec2_div(rf_vec2 v, float div); // Divide vector by a float value
rf_extern rf_vec2 rf_vec2_div_v(rf_vec2 v1, rf_vec2 v2); // Divide vector by vector
rf_extern rf_vec2 rf_vec2_normalize(rf_vec2 v); // Normalize provided vector
rf_extern rf_vec2 rf_vec2_lerp(rf_vec2 v1, rf_vec2 v2, float amount); // Calculate linear interpolation between two vectors

rf_extern rf_vec3 rf_vec3_add(rf_vec3 v1, rf_vec3 v2); // Add two vectors
rf_extern rf_vec3 rf_vec3_sub(rf_vec3 v1, rf_vec3 v2); // Subtract two vectors
rf_extern rf_vec3 rf_vec3_mul(rf_vec3 v, float scalar); // Multiply vector by scalar
rf_extern rf_vec3 rf_vec3_mul_v(rf_vec3 v1, rf_vec3 v2); // Multiply vector by vector
rf_extern rf_vec3 rf_vec3_cross_product(rf_vec3 v1, rf_vec3 v2); // Calculate two vectors cross product
rf_extern rf_vec3 rf_vec3_perpendicular(rf_vec3 v); // Calculate one vector perpendicular vector
rf_extern float rf_vec3_len(rf_vec3 v); // Calculate vector length
rf_extern float rf_vec3_dot_product(rf_vec3 v1, rf_vec3 v2); // Calculate two vectors dot product
rf_extern float rf_vec3_distance(rf_vec3 v1, rf_vec3 v2); // Calculate distance between two vectors
rf_extern rf_vec3 rf_vec3_scale(rf_vec3 v, float scale); // Scale provided vector
rf_extern rf_vec3 rf_vec3_negate(rf_vec3 v); // Negate provided vector (invert direction)
rf_extern rf_vec3 rf_vec3_div(rf_vec3 v, float div); // Divide vector by a float value
rf_extern rf_vec3 rf_vec3_div_v(rf_vec3 v1, rf_vec3 v2); // Divide vector by vector
rf_extern rf_vec3 rf_vec3_normalize(rf_vec3 v); // Normalize provided vector
rf_extern void rf_vec3_ortho_normalize(rf_vec3* v1, rf_vec3* v2); // Orthonormalize provided vectors. Makes vectors normalized and orthogonal to each other. Gram-Schmidt function implementation
rf_extern rf_vec3 rf_vec3_transform(rf_vec3 v, rf_mat mat); // Transforms a rf_vec3 by a given rf_mat
rf_extern rf_vec3 rf_vec3_rotate_by_quaternion(rf_vec3 v, rf_quaternion q); // rf_transform a vector by quaternion rotation
rf_extern rf_vec3 rf_vec3_lerp(rf_vec3 v1, rf_vec3 v2, float amount); // Calculate linear interpolation between two vectors
rf_extern rf_vec3 rf_vec3_reflect(rf_vec3 v, rf_vec3 normal); // Calculate reflected vector to normal
rf_extern rf_vec3 rf_vec3_min(rf_vec3 v1, rf_vec3 v2); // Return min value for each pair of components
rf_extern rf_vec3 rf_vec3_max(rf_vec3 v1, rf_vec3 v2); // Return max value for each pair of components
rf_extern rf_vec3 rf_vec3_barycenter(rf_vec3 p, rf_vec3 a, rf_vec3 b, rf_vec3 c); // Compute barycenter coordinates (u, v, w) for point p with respect to triangle (a, b, c) NOTE: Assumes P is on the plane of the triangle

rf_extern float rf_mat_determinant(rf_mat mat); // Compute matrix determinant
rf_extern float rf_mat_trace(rf_mat mat); // Returns the trace of the matrix (sum of the values along the diagonal)
rf_extern rf_mat rf_mat_transpose(rf_mat mat); // Transposes provided matrix
rf_extern rf_mat rf_mat_invert(rf_mat mat); // Invert provided matrix
rf_extern rf_mat rf_mat_normalize(rf_mat mat); // Normalize provided matrix
rf_extern rf_mat rf_mat_identity(void); // Returns identity matrix
rf_extern rf_mat rf_mat_add(rf_mat left, rf_mat right); // Add two matrices
rf_extern rf_mat rf_mat_sub(rf_mat left, rf_mat right); // Subtract two matrices (left - right)
rf_extern rf_mat rf_mat_translate(float x, float y, float z); // Returns translation matrix
rf_extern rf_mat rf_mat_rotate(rf_vec3 axis, float angle); // Create rotation matrix from axis and angle. NOTE: Angle should be provided in radians
rf_extern rf_mat rf_mat_rotate_xyz(rf_vec3 ang); // Returns xyz-rotation matrix (angles in radians)
rf_extern rf_mat rf_mat_rotate_x(float angle); // Returns x-rotation matrix (angle in radians)
rf_extern rf_mat rf_mat_rotate_y(float angle); // Returns y-rotation matrix (angle in radians)
rf_extern rf_mat rf_mat_rotate_z(float angle); // Returns z-rotation matrix (angle in radians)
rf_extern rf_mat rf_mat_scale(float x, float y, float z); // Returns scaling matrix
rf_extern rf_mat rf_mat_mul(rf_mat left, rf_mat right); // Returns two matrix multiplication. NOTE: When multiplying matrices... the order matters!
rf_extern rf_mat rf_mat_frustum(double left, double right, double bottom, double top, double near_val, double far_val); // Returns perspective GL_PROJECTION matrix
rf_extern rf_mat rf_mat_perspective(double fovy, double aspect, double near_val, double far_val); // Returns perspective GL_PROJECTION matrix. NOTE: Angle should be provided in radians
rf_extern rf_mat rf_mat_ortho(double left, double right, double bottom, double top, double near_val, double far_val); // Returns orthographic GL_PROJECTION matrix
rf_extern rf_mat rf_mat_look_at(rf_vec3 eye, rf_vec3 target, rf_vec3 up); // Returns camera look-at matrix (view matrix)
rf_extern rf_float16 rf_mat_to_float16(rf_mat mat); // Returns the matrix as an array of 16 floats

rf_extern rf_quaternion rf_quaternion_identity(void); // Returns identity quaternion
rf_extern float rf_quaternion_len(rf_quaternion q); // Computes the length of a quaternion
rf_extern rf_quaternion rf_quaternion_normalize(rf_quaternion q); // Normalize provided quaternion
rf_extern rf_quaternion rf_quaternion_invert(rf_quaternion q); // Invert provided quaternion
rf_extern rf_quaternion rf_quaternion_mul(rf_quaternion q1, rf_quaternion q2); // Calculate two quaternion multiplication
rf_extern rf_quaternion rf_quaternion_lerp(rf_quaternion q1, rf_quaternion q2, float amount); // Calculate linear interpolation between two quaternions
rf_extern rf_quaternion rf_quaternion_nlerp(rf_quaternion q1, rf_quaternion q2, float amount); // Calculate slerp-optimized interpolation between two quaternions
rf_extern rf_quaternion rf_quaternion_slerp(rf_quaternion q1, rf_quaternion q2, float amount); // Calculates spherical linear interpolation between two quaternions
rf_extern rf_quaternion rf_quaternion_from_vec3_to_vec3(rf_vec3 from, rf_vec3 to); // Calculate quaternion based on the rotation from one vector to another
rf_extern rf_quaternion rf_quaternion_from_mat(rf_mat mat); // Returns a quaternion for a given rotation matrix
rf_extern rf_mat rf_quaternion_to_mat(rf_quaternion q); // Returns a matrix for a given quaternion
rf_extern rf_quaternion rf_quaternion_from_axis_angle(rf_vec3 axis, float angle); // Returns rotation quaternion for an angle and axis. NOTE: angle must be provided in radians
rf_extern void rf_quaternion_to_axis_angle(rf_quaternion q, rf_vec3* outAxis, float* outAngle); // Returns the rotation angle and axis for a given quaternion
rf_extern rf_quaternion rf_quaternion_from_euler(float roll, float pitch, float yaw); // Returns he quaternion equivalent to Euler angles
rf_extern rf_vec3 rf_quaternion_to_euler(rf_quaternion q); // Return the Euler angles equivalent to quaternion (roll, pitch, yaw). NOTE: Angles are returned in a rf_vec3 struct in degrees
rf_extern rf_quaternion rf_quaternion_transform(rf_quaternion q, rf_mat mat); // rf_transform a quaternion given a transformation matrix

#endif // RAYFORK_MATHS_H