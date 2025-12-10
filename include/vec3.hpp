// include/vec3.hpp
#ifndef VEC3_HPP
#define VEC3_HPP

#include <cmath>
#include <iostream>
#include <algorithm>

class Vec3 {
public:
    double x, y, z;
    
    Vec3() : x(0), y(0), z(0) {}
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
    
    // Operadores aritméticos
    Vec3 operator+(const Vec3& v) const { return Vec3(x+v.x, y+v.y, z+v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x-v.x, y-v.y, z-v.z); }
    Vec3 operator*(double s) const { return Vec3(x*s, y*s, z*s); }
    Vec3 operator/(double s) const { return *this * (1.0/s); }
    Vec3 operator-() const { return Vec3(-x, -y, -z); }
    
    // Multiplicação componente a componente
    Vec3 mul(const Vec3& v) const { return Vec3(x*v.x, y*v.y, z*v.z); }
    
    // Produtos
    double dot(const Vec3& v) const { return x*v.x + y*v.y + z*v.z; }
    Vec3 cross(const Vec3& v) const { 
        return Vec3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x); 
    }
    
    // Métodos
    double length() const { return std::sqrt(x*x + y*y + z*z); }
    double lengthSquared() const { return x*x + y*y + z*z; }
    
    Vec3 normalize() const { 
        double len = length();
        return len > 0 ? *this / len : *this;
    }
    
    Vec3 reflect(const Vec3& normal) const {
        return *this - normal * (2.0 * this->dot(normal));
    }
    
    Vec3 clamp(double min = 0.0, double max = 1.0) const {
        return Vec3(
            std::clamp(x, min, max),
            std::clamp(y, min, max),
            std::clamp(z, min, max)
        );
    }
    
    // Debug
    friend std::ostream& operator<<(std::ostream& os, const Vec3& v) {
        return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    }
};

// Operador externo para s * v
inline Vec3 operator*(double s, const Vec3& v) { return v * s; }

#endif