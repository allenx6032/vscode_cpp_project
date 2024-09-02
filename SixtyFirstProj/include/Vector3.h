#ifndef __VECTOR_3_H__
#define __VECTOR_3_H__
#include <cmath>

class Vector3 {
public:
    float x, y, z;

    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}


    float magnitude() const {
        return std::sqrt((x * x) + (y * y) + (z * z));
    }


    Vector3 normalize() const {
        float mg = magnitude();
        if (mg != 0) {
            return Vector3(x/mg, y/mg, z/mg);
        }
    }

    // Calcula a distância entre dois vetores
    float distance(const Vector3& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    // Sobrecarga do operador de adição
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    // Sobrecarga do operador de subtração
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    bool operator==(const Vector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    // Calcula o produto interno entre dois vetores
    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    Vector3 cross(const Vector3& other) const {
        return Vector3(y * other.z - z * other.y, 
                       z * other.x - x * other.z, 
                       x * other.y - y * other.x
        );
    }

    Vector3 dist(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    float angle(const Vector3& other) const {
        float mag1 = magnitude();
        float mag2 = other.magnitude();

        if (mag1 == 0.0f || mag2 == 0.0f) {
            return 0.0f;
        }

        return std::acos(dot(other) / (mag1 * mag2));
    }


};

#endif
