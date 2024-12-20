#ifndef VECTOR3_H_
#define VECTOR3_H_

#include <ostream>
#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#define M_PI       3.14159265358979323846

template<typename T>
class Vector3
{
public:
	using type = T;

	//コンストラクタ
	Vector3()
		: x_()
		, y_()
		, z_()
	{
	}
	Vector3(const T &x, const T &y, const T &z)
		: x_(x)
		, y_(y)
		, z_(z)
	{
	}
	~Vector3(){}

	const T &getX() const { return x_; }
	const T &getY() const { return y_; }
	const T &getZ() const { return z_; }
	void setX(const T &x) { x_ = x; }
	void setY(const T &y) { y_ = y; }
	void setZ(const T &z) { z_ = z; }

	//単項演算子
	Vector3 &operator+=(const Vector3 &v)
	{
		 this->x_ += v.x_;
		 this->y_ += v.y_;
		 this->z_ += v.z_;
		 return *this;
	}
	Vector3 &operator-=(const Vector3 &v)
	{
		 this->x_ -= v.x_;
		 this->y_ -= v.y_;
		 this->z_ -= v.z_;
		 return *this;
	}
	Vector3 &operator*=(const T &k)
	{
		 this->x_ *= k;
		 this->y_ *= k;
		 this->z_ *= k;
		 return *this;
	}
	Vector3 &operator/=(const T &k)
	{
		 this->x_ /= k;
		 this->y_ /= k;
		 this->z_ /= k;
		 return *this;
	}
	Vector3 operator+()const
	{
		return *this;
	}
	Vector3 operator-()const
	{
		return Vector3(-x_,-y_,-z_);
	}
	//添え字演算子
	T& operator[](int i)
	{
		assert(0 <= i && i <= 2);
		switch(i) {
		case 0:
			return x_;
		case 1:
			return y_;
		case 2:
			return z_;
		default:
			throw i;
		}
	}
    //比較演算子
	bool operator==(const Vector3& v ) const { return (x_ == v.x_) && (y_ == v.y_) && (z_ == v.z_); }
	bool operator!=(const Vector3& v ) const { return !(*this == v); }
	//べクトルの長さ
	T norm()const { return std::pow((x_ * x_) + (y_ * y_) + (z_ * z_), 0.5); }
	//正規化
	void normalize() { *this /= norm(); }

private:
	//メンバ変数
	T x_;
	T y_;
	T z_;
};

// ベクトル演算

// Vector3 + Vector3
template<typename T>
inline Vector3<T> operator+(const Vector3<T> &u,const Vector3<T> &v)
{
	Vector3<T> w(u);
	w += v;
	return w;
}

// Vector3 - Vector3
template<typename T>
inline Vector3<T> operator-(const Vector3<T> &u,const Vector3<T> &v)
{
	Vector3<T> w(u);
	w -= v;
	return w;
}

// T * Vector3
template<typename T>
Vector3<T> operator*(const T &k, const Vector3<T> &v)
{
	Vector3<T> w(v);
	w *= k;
	return w;
}

// Vector3 * T
template<typename T>
Vector3<T> operator*(const Vector3<T> &v, const T &k)
{
	Vector3<T> w(v);
	w *= k;
	return w;
}

// Vector3 / T
template<typename T>
Vector3<T> operator/(const Vector3<T> &v, const T &k)
{
	Vector3<T> w(v);
	w /= k;
	return w;
}

// 内積 Vector3 * Vector3
template<typename T>
T operator*(const Vector3<T> &u, const Vector3<T> v)
{
	return u.getX() * v.getX() + u.getY() * v.getY() + u.getZ() * v.getZ();
}

// 外積 Vector3 % Vector3
template<typename T>
Vector3<T> operator%(const Vector3<T> &u, const Vector3<T> &v)
{
	return {u.getY() * v.getZ() - u.getZ() * v.getY(),
		u.getZ() * v.getX() - u.getX() * v.getZ(),
		u.getX() * v.getY() - u.getY() * v.getX()};
	//	Vector3<T> w;
//	w.x_ = u.y_ * v.z_ - u.z_ * v.y_;
//	w.y_ = u.z_ * v.x_ - u.x_ * v.z_;
//	w.z_ = u.x_ * v.y_ - u.y_ * v.x_;
//	return w;
}

//2つのベクトルのなす角度
template<typename T>
T angle(const Vector3<T> &u,const Vector3<T> &v)
{
	T cos = (u * v) / (u.norm() * v.norm());
	return (std::acos(cos) / M_PI * 180.0);
}

//出力
template<typename T>
inline std::ostream& operator<<(std::ostream& s, const Vector3<T> &v)
{
	return s << '[' << v.getX() << ", " << v.getY() << ", " << v.getZ() << ']';
}

using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;

#endif // VECTOR3_H_
