// include/intersect.hpp
#ifndef INTERSECT_HPP
#define INTERSECT_HPP

#include "scene.hpp"

// Função principal
HitInfo findClosestHit(const Ray& ray, const Scene& scene);

// Funções auxiliares de interseção
namespace Intersect {
    bool sphere(const Ray& ray, const Object& obj, HitInfo& hit);
    bool polyhedron(const Ray& ray, const Object& obj, HitInfo& hit);
    bool triangle(const Ray& ray, const Object& obj, HitInfo& hit);
    bool cylinder(const Ray& ray, const Object& obj, HitInfo& hit);
    bool cone(const Ray& ray, const Object& obj, HitInfo& hit);
    bool quadric(const Ray& ray, const Object& obj, HitInfo& hit);
    
    // Função genérica para resolver equação quadrática
    bool solveQuadratic(double a, double b, double c, double& t1, double& t2);
    
    // Ajustar normal para apontar contra o raio
    void adjustNormal(Vec3& normal, const Vec3& rayDir);
}

#endif
