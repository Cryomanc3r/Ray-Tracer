// src/intersect.cpp
#include "../include/intersect.hpp"
#include <limits>
#include <cmath>

namespace Intersect {

// Resolver equação quadrática: at² + bt + c = 0
bool solveQuadratic(double a, double b, double c, double& t1, double& t2) {
    double discriminant = b*b - 4*a*c;
    if (discriminant < 0) return false;
    
    double sqrtd = std::sqrt(discriminant);
    t1 = (-b - sqrtd) / (2*a);
    t2 = (-b + sqrtd) / (2*a);
    return true;
}

// Ajustar normal para apontar contra o raio
void adjustNormal(Vec3& normal, const Vec3& rayDir) {
    if (normal.dot(rayDir) > 0) {
        normal = -normal;
    }
}

// Interseção com esfera
bool sphere(const Ray& ray, const Object& obj, HitInfo& hit) {
    const Vec3& center = obj.sphere.center;
    double radius = obj.sphere.radius;
    
    Vec3 oc = ray.origin - center;
    double a = ray.direction.dot(ray.direction);
    double b = 2.0 * oc.dot(ray.direction);
    double c = oc.dot(oc) - radius * radius;
    
    double t1, t2;
    if (!solveQuadratic(a, b, c, t1, t2)) return false;
    
    double t = (t1 > EPSILON) ? t1 : t2;
    if (t < EPSILON) return false;
    
    hit.hit = true;
    hit.t = t;
    hit.point = ray.at(t);
    hit.normal = (hit.point - center).normalize();
    adjustNormal(hit.normal, ray.direction);
    
    return true;
}

// Interseção com poliedro convexo
bool polyhedron(const Ray& ray, const Object& obj, HitInfo& hit) {
    double tNear = -std::numeric_limits<double>::max();
    double tFar = std::numeric_limits<double>::max();
    Vec3 nearNormal;
    bool foundNear = false;
    
    for (const auto& plane : obj.faces) {
        Vec3 n = plane.normal();
        double denom = n.dot(ray.direction);
        double num = -(n.dot(ray.origin) + plane.d);
        
        if (std::fabs(denom) < EPSILON) {
            if (num < 0) return false;
            continue;
        }
        
        double t = num / denom;
        
        if (denom < 0) {
            // Entrando
            if (t > tNear) {
                tNear = t;
                nearNormal = n;
                foundNear = true;
            }
        } else {
            // Saindo
            if (t < tFar) {
                tFar = t;
            }
        }
        
        if (tNear > tFar) return false;
    }
    
    // Se tNear é negativo, usar tFar (estamos dentro)
    if (tNear < EPSILON) {
        tNear = tFar;
        // Encontrar normal da face de saída
        for (const auto& plane : obj.faces) {
            Vec3 n = plane.normal();
            double denom = n.dot(ray.direction);
            if (denom > EPSILON) {
                double num = -(n.dot(ray.origin) + plane.d);
                double t = num / denom;
                if (std::fabs(t - tNear) < EPSILON) {
                    nearNormal = -n;
                    foundNear = true;
                    break;
                }
            }
        }
    }
    
    if (tNear < EPSILON || !foundNear) return false;
    
    hit.hit = true;
    hit.t = tNear;
    hit.point = ray.at(tNear);
    hit.normal = nearNormal.normalize();
    adjustNormal(hit.normal, ray.direction);
    
    return true;
}

// Interseção com triângulo (Möller-Trumbore)
bool triangle(const Ray& ray, const Object& obj, HitInfo& hit) {
    const Vec3& v0 = obj.triangle.v0;
    const Vec3& v1 = obj.triangle.v1;
    const Vec3& v2 = obj.triangle.v2;
    
    Vec3 e1 = v1 - v0;
    Vec3 e2 = v2 - v0;
    Vec3 h = ray.direction.cross(e2);
    double a = e1.dot(h);
    
    if (std::fabs(a) < EPSILON) return false;
    
    double f = 1.0 / a;
    Vec3 s = ray.origin - v0;
    double u = f * s.dot(h);
    
    if (u < 0.0 || u > 1.0) return false;
    
    Vec3 q = s.cross(e1);
    double v = f * ray.direction.dot(q);
    
    if (v < 0.0 || u + v > 1.0) return false;
    
    double t = f * e2.dot(q);
    if (t < EPSILON) return false;
    
    hit.hit = true;
    hit.t = t;
    hit.point = ray.at(t);
    hit.normal = e1.cross(e2).normalize();
    adjustNormal(hit.normal, ray.direction);
    
    return true;
}

// Função auxiliar para cilindro e cone
template<typename CheckFunc>
bool intersectCylindrical(const Ray& ray, double a_coef, double b_coef, double c_coef,
                          CheckFunc checkHeight, HitInfo& hit) {
    double t1, t2;
    if (!solveQuadratic(a_coef, b_coef, c_coef, t1, t2)) return false;
    
    double t = -1;
    if (checkHeight(t1)) t = t1;
    else if (checkHeight(t2)) t = t2;
    
    if (t < EPSILON) return false;
    
    hit.hit = true;
    hit.t = t;
    hit.point = ray.at(t);
    return true;
}

// Interseção com cilindro
bool cylinder(const Ray& ray, const Object& obj, HitInfo& hit) {
    const Vec3& base = obj.cylinderCone.base;
    Vec3 axis = obj.cylinderCone.axis.normalize();
    double radius = obj.cylinderCone.radius1;
    double height = obj.cylinderCone.height;
    
    Vec3 oc = ray.origin - base;
    double axis_dot_dir = ray.direction.dot(axis);
    double axis_dot_oc = oc.dot(axis);
    
    double a = ray.direction.dot(ray.direction) - axis_dot_dir * axis_dot_dir;
    double b = 2.0 * (ray.direction.dot(oc) - axis_dot_dir * axis_dot_oc);
    double c = oc.dot(oc) - axis_dot_oc * axis_dot_oc - radius * radius;
    
    auto checkHeight = [&](double t) -> bool {
        if (t < EPSILON) return false;
        Vec3 p = ray.at(t);
        double h = (p - base).dot(axis);
        return (h >= 0 && h <= height);
    };
    
    if (!intersectCylindrical(ray, a, b, c, checkHeight, hit)) 
        return false;
    
    Vec3 op = hit.point - base;
    double proj = op.dot(axis);
    hit.normal = (op - axis * proj).normalize();
    adjustNormal(hit.normal, ray.direction);
    
    return true;
}

// Interseção com cone
bool cone(const Ray& ray, const Object& obj, HitInfo& hit) {
    const Vec3& apex = obj.cylinderCone.base;
    Vec3 axis = obj.cylinderCone.axis.normalize();
    double height = obj.cylinderCone.height;
    double radius = obj.cylinderCone.radius1;
    double k = radius / height;
    double k2 = 1 + k*k;
    
    Vec3 oc = ray.origin - apex;
    double axis_dot_dir = ray.direction.dot(axis);
    double axis_dot_oc = oc.dot(axis);
    
    double a = ray.direction.dot(ray.direction) - k2 * axis_dot_dir * axis_dot_dir;
    double b = 2.0 * (ray.direction.dot(oc) - k2 * axis_dot_dir * axis_dot_oc);
    double c = oc.dot(oc) - k2 * axis_dot_oc * axis_dot_oc;
    
    auto checkHeight = [&](double t) -> bool {
        if (t < EPSILON) return false;
        Vec3 p = ray.at(t);
        double h = (p - apex).dot(axis);
        return (h >= 0 && h <= height);
    };
    
    if (!intersectCylindrical(ray, a, b, c, checkHeight, hit))
        return false;
    
    Vec3 op = hit.point - apex;
    double h = op.dot(axis);
    Vec3 proj = axis * h;
    hit.normal = (op - proj * k2).normalize();
    adjustNormal(hit.normal, ray.direction);
    
    return true;
}

// Interseção com quádrica
bool quadric(const Ray& ray, const Object& obj, HitInfo& hit) {
    const auto& q = obj.quadric;
    const Vec3& o = ray.origin;
    const Vec3& d = ray.direction;
    
    double Aq = q.A*d.x*d.x + q.B*d.y*d.y + q.C*d.z*d.z +
                q.D*d.x*d.y + q.E*d.x*d.z + q.F*d.y*d.z;
    
    double Bq = 2.0*q.A*o.x*d.x + 2.0*q.B*o.y*d.y + 2.0*q.C*o.z*d.z +
                q.D*(o.x*d.y + o.y*d.x) + q.E*(o.x*d.z + o.z*d.x) +
                q.F*(o.y*d.z + o.z*d.y) + q.G*d.x + q.H*d.y + q.I*d.z;
    
    double Cq = q.A*o.x*o.x + q.B*o.y*o.y + q.C*o.z*o.z +
                q.D*o.x*o.y + q.E*o.x*o.z + q.F*o.y*o.z +
                q.G*o.x + q.H*o.y + q.I*o.z + q.J;
    
    double t1, t2;
    if (!solveQuadratic(Aq, Bq, Cq, t1, t2)) return false;
    
    double t = (t1 > EPSILON) ? t1 : t2;
    if (t < EPSILON) return false;
    
    hit.hit = true;
    hit.t = t;
    hit.point = ray.at(t);
    
    const Vec3& p = hit.point;
    hit.normal = Vec3(
        2.0*q.A*p.x + q.D*p.y + q.E*p.z + q.G,
        2.0*q.B*p.y + q.D*p.x + q.F*p.z + q.H,
        2.0*q.C*p.z + q.E*p.x + q.F*p.y + q.I
    ).normalize();
    
    adjustNormal(hit.normal, ray.direction);
    return true;
}

} // namespace Intersect

// Função principal
HitInfo findClosestHit(const Ray& ray, const Scene& scene) {
    HitInfo closest;
    closest.t = std::numeric_limits<double>::max();
    
    // Tabela de funções de interseção
    using IntersectFunc = bool(*)(const Ray&, const Object&, HitInfo&);
    static const IntersectFunc intersectFuncs[] = {
        Intersect::sphere,      // SPHERE
        Intersect::polyhedron,  // POLYHEDRON
        Intersect::quadric,     // QUADRIC
        Intersect::triangle,    // TRIANGLE
        Intersect::cylinder,    // CYLINDER
        Intersect::cone         // CONE
    };
    
    for (size_t i = 0; i < scene.objects.size(); i++) {
        const Object& obj = scene.objects[i];
        HitInfo hit;
        
        if (intersectFuncs[obj.type](ray, obj, hit) && hit.t < closest.t) {
            closest = hit;
            closest.objectIdx = i;
        }
    }
    
    return closest;
}