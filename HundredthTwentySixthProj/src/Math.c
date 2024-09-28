#include "Math.h"

float Clamp(float x, float min, float max)
{
	if (x < min)
		return min;
	if (x > max)
		return max;
	return x;
}

vec3 Vec3(float a, float b, float c)
{
	vec3 v;
	v.x = a;
	v.y = b;
	v.z = c;

	return v;
}

float LenV3(vec3 v)
{
	return (float)sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3 NormalizedV3(vec3 v)
{
	float l = LenV3(v);
	return Vec3(v.x / l, v.y / l, v.z / l);
}

vec3 subV3(vec3 a, vec3 b)
{
	return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

vec3 addV3(vec3 a, vec3 b)
{
	return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec3 cross(vec3 a, vec3 b)
{
	return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

vec3 minusVec(vec3 v)
{
	v.x *= -1;
	v.y *= -1;
	v.z *= -1;
	return v;
}

float dotV3(vec3 a, vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3i Vec3i(int a, int b, int c)
{
	vec3i v;
	v.x = a;
	v.y = b;
	v.z = c;

	return v;
}

vec2 Vec2(int a, int b)
{
	vec2 v;
	v.x = a;
	v.y = b;

	return v;
}

vec4 Vec4(float a, float b, float c, float d)
{
	vec4 v;
	v.x = a;
	v.y = b;
	v.z = c;
	v.w = d;
	return v;
}

mat4 Identity()
{
	return Mat4m(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}

mat4 Mat4(vec4 a, vec4 b, vec4 c, vec4 d)
{
	mat4 m;
	m.m00 = a.x;
	m.m01 = a.y;
	m.m02 = a.z;
	m.m03 = a.w;

	m.m10 = b.x;
	m.m11 = b.y;
	m.m12 = b.z;
	m.m13 = b.w;

	m.m20 = c.x;
	m.m21 = c.y;
	m.m22 = c.z;
	m.m23 = c.w;

	m.m30 = d.x;
	m.m31 = d.y;
	m.m32 = d.z;
	m.m33 = d.w;
	return m;
}

mat4 Mat4m(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
{
	mat4 m = Mat4(
		Vec4(m00, m01, m02, m03),
		Vec4(m10, m11, m12, m13),
		Vec4(m20, m21, m22, m23),
		Vec4(m30, m31, m32, m33));
	return m;
}

mat4 PerspectiveProj(float FOV, float Screen, float near, float far)
{
	float xScale, yScale, C, D;
	yScale = 1.0f / (float)tan((FOV * deg2rad) / 2.0f);
	xScale = yScale / Screen;

	C = far / (far - near);
	D = -near * far / (far - near);

	return Mat4m(
		xScale, 0, 0, 0,
		0, yScale, 0, 0,
		0, 0, C, 1,
		0, 0, D, 0);
}

mat4 LookAt(vec3 eye, vec3 at, vec3 up)
{
	vec3 zaxis, xaxis, yaxis;
	vec3 t;
	t.x = at.x - eye.x;
	t.y = at.y - eye.y;
	t.z = at.z - eye.z;
	zaxis = NormalizedV3(t);
	xaxis = NormalizedV3(cross(up, zaxis));
	yaxis = cross(zaxis, xaxis);

	return Mat4m(
		xaxis.x, yaxis.x, zaxis.x, 0,
		xaxis.y, yaxis.y, zaxis.y, 0,
		xaxis.z, yaxis.z, zaxis.z, 0,
		-1 * dotV3(xaxis, eye), -1 * dotV3(yaxis, eye), -1 * dotV3(zaxis, eye), 1);
}

mat4 LookAtCam(Camera cam)
{
	mat4 camRot = RotMatrix(cam.transform.rotation);
	vec4 at = MultMatVec(Vec4(0, 0, 1, 1), camRot);
	vec4 up = MultMatVec(Vec4(0, 1, 0, 1), camRot);
	// printf("At: %f,%f,%f; Up: %f,%f,%f\n",at.x,at.y,at.z,up.x,up.y,up.z);
	return LookAt(cam.transform.position, addV3(cam.transform.position, Vec3(at.x, at.y, at.z)), Vec3(up.x, up.y, up.z));
}

mat4 RotMatrix(vec3 rot)
{
	vec3 r;
	r.x = rot.x * deg2rad;
	r.y = rot.y * deg2rad;
	r.z = rot.z * deg2rad;

	mat4 roll = Mat4m(
		1, 0, 0, 0,
		0, cos(r.x), -sin(r.x), 0,
		0, sin(r.x), cos(r.x), 0,
		0, 0, 0, 1);
	mat4 pitch = Mat4m(
		cos(r.y), 0, sin(r.y), 0,
		0, 1, 0, 0,
		-sin(r.y), 0, cos(r.y), 0,
		0, 0, 0, 1);

	mat4 yaw = Mat4m(
		cos(r.z), -sin(r.z), 0, 0,
		sin(r.z), cos(r.z), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	return MultMat(MultMat(yaw, pitch), roll);
}
mat4 MultMat(mat4 A, mat4 B)
{
	return Mat4m(
		A.m00 * B.m00 + A.m01 * B.m10 + A.m02 * B.m20 + A.m03 * B.m30, A.m00 * B.m01 + A.m01 * B.m11 + A.m02 * B.m21 + A.m03 * B.m31, A.m00 * B.m02 + A.m01 * B.m12 + A.m02 * B.m22 + A.m03 * B.m32, A.m00 * B.m03 + A.m01 * B.m13 + A.m02 * B.m23 + A.m03 * B.m33,
		A.m10 * B.m00 + A.m11 * B.m10 + A.m12 * B.m20 + A.m13 * B.m30, A.m10 * B.m01 + A.m11 * B.m11 + A.m12 * B.m21 + A.m13 * B.m31, A.m10 * B.m02 + A.m11 * B.m12 + A.m12 * B.m22 + A.m13 * B.m32, A.m10 * B.m03 + A.m11 * B.m13 + A.m12 * B.m23 + A.m13 * B.m33,
		A.m20 * B.m00 + A.m21 * B.m10 + A.m22 * B.m20 + A.m23 * B.m30, A.m20 * B.m01 + A.m21 * B.m11 + A.m22 * B.m21 + A.m23 * B.m31, A.m20 * B.m02 + A.m21 * B.m12 + A.m22 * B.m22 + A.m23 * B.m32, A.m20 * B.m03 + A.m21 * B.m13 + A.m22 * B.m23 + A.m23 * B.m33,
		A.m30 * B.m00 + A.m31 * B.m10 + A.m32 * B.m20 + A.m33 * B.m30, A.m30 * B.m01 + A.m31 * B.m11 + A.m32 * B.m21 + A.m33 * B.m31, A.m30 * B.m02 + A.m31 * B.m12 + A.m32 * B.m22 + A.m33 * B.m32, A.m30 * B.m03 + A.m31 * B.m13 + A.m32 * B.m23 + A.m33 * B.m33);
}

vec4 MultMatVec(vec4 v, mat4 A)
{
	return Vec4(
		A.m00 * v.x + A.m01 * v.y + A.m02 * v.z + A.m03 * v.w,
		A.m10 * v.x + A.m11 * v.y + A.m12 * v.z + A.m13 * v.w,
		A.m20 * v.x + A.m21 * v.y + A.m22 * v.z + A.m23 * v.w,
		A.m30 * v.x + A.m31 * v.y + A.m32 * v.z + A.m33 * v.w);
}

mat4 MultMatFloat(mat4 M, float f)
{
	return Mat4m(
		M.m00 * f, M.m01 * f, M.m02 * f, M.m03 * f,
		M.m10 * f, M.m11 * f, M.m12 * f, M.m13 * f,
		M.m20 * f, M.m21 * f, M.m22 * f, M.m23 * f,
		M.m30 * f, M.m31 * f, M.m32 * f, M.m33 * f);
}

float matTrace(mat4 M)
{
	return M.m00 + M.m11 + M.m22 + M.m33;
}

mat4 MatSub(mat4 A, mat4 B)
{
	return Mat4m(
		A.m00 - B.m00, A.m01 - B.m01, A.m02 - B.m02, A.m03 - B.m03,
		A.m10 - B.m10, A.m11 - B.m11, A.m12 - B.m12, A.m13 - B.m13,
		A.m20 - B.m20, A.m21 - B.m21, A.m22 - B.m22, A.m23 - B.m23,
		A.m30 - B.m30, A.m31 - B.m31, A.m32 - B.m32, A.m33 - B.m33);
}
mat4 MatAdd(mat4 A, mat4 B)
{
	return Mat4m(
		A.m00 + B.m00, A.m01 + B.m01, A.m02 + B.m02, A.m03 + B.m03,
		A.m10 + B.m10, A.m11 + B.m11, A.m12 + B.m12, A.m13 + B.m13,
		A.m20 + B.m20, A.m21 + B.m21, A.m22 + B.m22, A.m23 + B.m23,
		A.m30 + B.m30, A.m31 + B.m31, A.m32 + B.m32, A.m33 + B.m33);
}

float Det(mat4 M)
{
	return M.m03 * M.m12 * M.m21 * M.m30 - M.m02 * M.m13 * M.m21 * M.m30 -
		   M.m03 * M.m11 * M.m22 * M.m30 + M.m01 * M.m13 * M.m22 * M.m30 +
		   M.m02 * M.m11 * M.m23 * M.m30 - M.m01 * M.m12 * M.m23 * M.m30 -
		   M.m03 * M.m12 * M.m20 * M.m31 + M.m02 * M.m13 * M.m20 * M.m31 +
		   M.m03 * M.m10 * M.m22 * M.m31 - M.m00 * M.m13 * M.m22 * M.m31 -
		   M.m02 * M.m10 * M.m23 * M.m31 + M.m00 * M.m12 * M.m23 * M.m31 +
		   M.m03 * M.m11 * M.m20 * M.m32 - M.m01 * M.m13 * M.m20 * M.m32 -
		   M.m03 * M.m10 * M.m21 * M.m32 + M.m00 * M.m13 * M.m21 * M.m32 +
		   M.m01 * M.m10 * M.m23 * M.m32 - M.m00 * M.m11 * M.m23 * M.m32 -
		   M.m02 * M.m11 * M.m20 * M.m33 + M.m01 * M.m12 * M.m20 * M.m33 +
		   M.m02 * M.m10 * M.m21 * M.m33 - M.m00 * M.m12 * M.m21 * M.m33 -
		   M.m01 * M.m10 * M.m22 * M.m33 + M.m00 * M.m11 * M.m22 * M.m33;
}

// Doesn't work currently, might fix later
mat4 InvertMat(mat4 This)
{

	mat4 Squared = MultMat(This, This);
	mat4 Cubed = MultMat(Squared, This);

	mat4 A, B, C, D;
	float t1 = matTrace(This);
	float t2 = matTrace(Squared);
	float t3 = matTrace(Cubed);

	// A = MultMatFloat(Identity(),0.16666666666666667f * (float)(pow(matTrace(This),3)) - 3 * matTrace(This) *matTrace(Squared) + 2 *matTrace(Cubed));

	A = MultMatFloat(Identity(), 0.16666666666666666666666666666667f * (t1 * t1 * t1) - (3.0f * t1 * t2) + (2.0f * t3));

	B = MultMatFloat(This, (0.5f * t1 * t1) - t2);

	C = MultMatFloat(Squared, t1);

	D = MultMatFloat(MatAdd(MatSub(A, B), MatSub(C, Cubed)), 1.0f / Det(This));

	// B = MultMatFloat(This , 0.5f * (float)pow(matTrace(This),2) - matTrace(Squared));

	// C = MultMatFloat(Squared,matTrace(This));

	// D = MultMatFloat( MatAdd( MatSub(A,B),MatSub(C,Cubed) ),(1.0f / Det(This)));

	return D;
}

mat4 TransformMat(vec3 pos, vec3 r, vec3 s)
{
	mat4 trans = Identity();
	trans.m03 = pos.x;
	trans.m13 = pos.y;
	trans.m23 = pos.z;
	mat4 rot = RotMatrix(r);
	mat4 scale = Identity();
	// scale.m00 = s.x;
	// scale.m11 = s.x;
	// scale.m22 = s.x;
	// scale.m33 = 1;
	// return MultMat(MultMat(scale, rot), trans);
	return MultMat(MultMat(scale, rot), trans);
}

// Made using wolfram Alpha, i would've never been able to do something like this
// TODO change transformation order: m((scale -> rot) -> trans) => ((rot -> trans) -> scale)
mat4 STransformMat(const Transform* t)
{
	float X =  t->rotation.x * deg2rad;
	float Y =  t->rotation.y * deg2rad;
	float Z =  t->rotation.z * deg2rad;
	
	float
		x = t->scale.x,
		y = t->scale.y, z = t->scale.z,
		m = t->position.x, n = t->position.y, p = t->position.z,
		sX = sin(X), sY = sin(Y), sZ = sin(Z),
		cX = cos(X), cY = cos(Y), cZ = cos(Z);

	return Mat4m(
		x * cY * cZ, x * (sX * sY * cZ - cX * sZ), x * (cX * sY * cZ + sX * sZ), m * x * cY * cZ + n * x * (sX * sY * cZ - cX * sZ) + p * x * (cX * sY * cZ + sX * sZ),
		y * cY * sZ, y * (sX * sY * sZ + cX * cZ), y * (cX * sY * sZ - sX * cZ), m * y * cY * sZ + n * y * (sX * sY * sZ + cX * cZ) + p * y * (cX * sY * sZ - sX * cZ),
		-z * sY, z * sX * cY, z * cX * cY, -m * z * sY + n * z * sX * cY + p * z * cX * cY,
		0, 0, 0, 1);
}

// mat4 STransformMatB(vec3 pos, vec3 r, vec3 s)
// {
// 	float x = s.x, y = s.y, z = s.z, m = pos.x, n = pos.y, p = pos.z;
// 	float
// 	sX = sin(r.x),sY = sin(r.y),sZ = sin(r.z),
// 	cX = cos(r.x),cY = cos(r.y),cZ = cos(r.z);

// 	return Mat4m(
// 		 x * cos(r.y) * cos(r.z), x * (sin(r.x) * sin(r.y) * cos(r.z) - cos(r.x) * sin(r.z)), x * (cos(r.x) * sin(r.y) * cos(r.z) + sin(r.x) * sin(r.z)), m * x * cos(r.y) * cos(r.z) + n * x * (sin(r.x) * sin(r.y) * cos(r.z) - cos(r.x) * sin(r.z)) + p * x * (cos(r.x) * sin(r.y) * cos(r.z) + sin(r.x) * sin(r.z)),
// 		 y * cos(r.y) * sin(r.z), y * (sin(r.x) * sin(r.y) * sin(r.z) + cos(r.x) * cos(r.z)), y * (cos(r.x) * sin(r.y) * sin(r.z) - sin(r.x) * cos(r.z)), m * y * cos(r.y) * sin(r.z) + n * y * (sin(r.x) * sin(r.y) * sin(r.z) + cos(r.x) * cos(r.z)) + p * y * (cos(r.x) * sin(r.y) * sin(r.z) - sin(r.x) * cos(r.z)),
// 		-z * sin(r.y), z * sin(r.x) * cos(r.y), z * cos(r.x) * cos(r.y), -m * z * sin(r.y) + n * z * sin(r.x) * cos(r.y) + p * z * cos(r.x) * cos(r.y),
// 		 0, 0, 0, 1);
// }

mat4 PerspectiveGLU(float fov, float aspect, float near, float far)
{
	float f = cos(fov / 2) / sin(fov / 2);
	return Mat4m(
		f / aspect, 0, 0, 0,
		0, f, 0, 0,
		0, 0, (far + near) / (near + far), (2 * far * near) / (near - far),
		0, 0, -1, 0);
}