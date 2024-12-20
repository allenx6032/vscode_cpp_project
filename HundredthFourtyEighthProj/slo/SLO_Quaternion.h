#ifndef GRAPHICS_QUATERNION_H_
#define GRAPHICS_QUATERNION_H_

#include "SLO_Vector3.h"
//#include "../Misc.h"

class Quaternion
{
public:
	Quaternion()
		: w_(1.0)
		, v_()
	{
	}

	Quaternion(double w, double x, double y, double z)
		: w_(w)
		, v_(x, y, z)
	{
	}

	Quaternion(double w, const Vector3d &v)
		: w_(w)
		, v_(v)
	{
	}

	~Quaternion()
	{
	}

	double getW() const { return w_; }
	double getX() const { return v_.getX(); }
	double getY() const { return v_.getY(); }
	double getZ() const { return v_.getZ(); }
	const Vector3d &getV() const { return v_; }

	void setW(double w) { w_ = w; }
	void setX(double x) { v_.setX(x); }
	void setY(double y) { v_.setY(y); }
	void setZ(double z) { v_.setZ(z); }
	void setV(const Vector3d &v) { v_ = v; }

	void identity()
	{
		setW(1.0);
		setV(Vector3d());
	}
	Quaternion inverse() const { return {getW(), -getV()}; }
	void normalize() { v_.normalize(); }
	static Quaternion rotate(double degrees, const Vector3d &axis)
	{
		//原点を中心とした軸ベクトル = (x, y, z)
		//回転する角度 = th
		//Q = (cos(th / 2); x * sin(th / 2), y * sin(th / 2), z * sin(th / 2))
		const double th2 = degrees2radian(degrees * 0.5);
		return {std::cos(th2), axis * std::sin(th2)};
	}

private:
	double w_;
	Vector3d v_;
};

inline Quaternion operator*(const Quaternion &lh, const Quaternion &rh)
{
	// Q = (q; V)
	// R = (r; W)
	// QR = (qr - V・W; qW + rV + V × W)
	return {lh.getW() * rh.getW() - Vector3d::dot(lh.getV(), rh.getV()),
		lh.getW() * rh.getV() + rh.getW() * lh.getV() + Vector3d::cross(lh.getV(), rh.getV())
	};
}

#endif // GRAPHICS_QUATERNION_H_
