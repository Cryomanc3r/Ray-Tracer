// src/scene.cpp
#include "../include/scene.hpp"
#include <cmath>

Plane::Plane(double a, double b, double c, double d) 
    : a(a), b(b), c(c), d(d) {
    normalize();
}

void Plane::normalize() {
    double len = std::sqrt(a*a + b*b + c*c);
    if (len > 0) {
        a /= len; 
        b /= len; 
        c /= len; 
        d /= len;
    }
}