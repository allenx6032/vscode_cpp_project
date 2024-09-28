#ifndef _MATH_H_
#define _MATH_H_
#include <math.h>
#include "global.h"

float Clamp(float x,float min, float max);

vec3 Vec3(float a, float b, float c);
float LenV3(vec3 v);
vec3 NormalizedV3(vec3 v);
vec3 subV3(vec3 a, vec3 b);
vec3 addV3(vec3 a, vec3 b);
vec3 cross(vec3 a, vec3 b);
vec3 minusVec(vec3 v);


float dotV3(vec3 a, vec3 b);

vec3i Vec3i(int a, int b, int c);
vec2 Vec2(int a, int b);

vec4 Vec4(float a, float b, float c, float d);

mat4 Identity();
mat4 Mat4(vec4 a, vec4 b, vec4 c, vec4 d);
mat4 Mat4m(
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33);
mat4 PerspectiveProj(float FOV, float Screen, float ScreenNear, float ScreenFar);
mat4 LookAt(vec3 eye, vec3 at, vec3 up);
mat4 RotMatrix(vec3 rot);
mat4 MultMat(mat4 A, mat4 B);
vec4 MultMatVec(vec4 v, mat4 mat);
mat4 LookAtCam(Camera cam);
mat4 MultMatFloat(mat4 M,float f);
float matTrace(mat4 M);
mat4 MatSub(mat4 A,mat4 B);
mat4 MatAdd(mat4 A,mat4 B);

float Det(mat4 M);
mat4 InvertMat(mat4 M);

mat4 TransformMat(vec3 pos,vec3 rot, vec3 scale);
mat4 STransformMat(const Transform* t);
mat4 STransformMatB(vec3 pos,vec3 rot, vec3 scale);
mat4 PerspectiveGLU(float fov,float aspect, float near,float far);



#endif