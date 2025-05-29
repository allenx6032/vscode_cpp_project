//Vecotr3.h
#pragma once
 
extern const double uZero;
 
class Vector3
{
	float x, y, z;
public:
	Vector3():x(0), y(0), z(0){}
	Vector3(float x1, float y1, float z1):x(x1), y(y1), z(z1){}
	Vector3(const Vector3 &v);
	~Vector3();
	void operator=(const Vector3 &v);
	Vector3 operator+(const Vector3 &v);
	Vector3 operator-(const Vector3 &v);
	Vector3 operator/(const Vector3 &v);
	Vector3 operator*(const Vector3 &v);
	Vector3 operator+(float f);
	Vector3 operator-(float f);
	Vector3 operator/(float f);
	Vector3 operator*(float f);
	float dot(const Vector3 &v);
	float length();
	void normalize();
	Vector3 crossProduct(const Vector3 &v);
	void printVec3();
};