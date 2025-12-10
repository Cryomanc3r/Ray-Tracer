// include/scene.hpp
#ifndef SCENE_HPP
#define SCENE_HPP

#include "vec3.hpp"
#include <vector>
#include <string>

// Constantes
constexpr double EPSILON = 1e-6;

// Ray
struct Ray {
    Vec3 origin;
    Vec3 direction;
    
    Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d.normalize()) {}
    Vec3 at(double t) const { return origin + direction * t; }
};

// Pigment
enum PigmentType { SOLID, CHECKER, TEXMAP };

struct Pigment {
    PigmentType type = SOLID;
    Vec3 color1 = Vec3(1, 1, 1);
    Vec3 color2 = Vec3(0, 0, 0);
    double scale = 1.0;
    
    // Textura
    std::string texturePath;
    double p0[4] = {0, 0, 0, 0};
    double p1[4] = {0, 0, 0, 0};
    std::vector<Vec3> textureData;
    int textureWidth = 0, textureHeight = 0;
};

// Finish
struct Finish {
    double ka = 0.1;    // Ambiente
    double kd = 0.7;    // Difuso
    double ks = 0.2;    // Especular
    double alpha = 50.0; // Expoente especular
    double kr = 0.0;    // Reflexão
    double kt = 0.0;    // Transmissão
    double ior = 1.5;   // Índice refração
};

// Plano
struct Plane {
    double a, b, c, d;
    
    Plane(double a = 0, double b = 0, double c = 1, double d = 0);
    void normalize();
    Vec3 normal() const { return Vec3(a, b, c); }
    double distance(const Vec3& p) const { return a*p.x + b*p.y + c*p.z + d; }
};

// Tipos de objetos
enum ObjectType { SPHERE, POLYHEDRON, QUADRIC, TRIANGLE, CYLINDER, CONE };

// Dados específicos de objetos
struct SphereData {
    Vec3 center = Vec3(0, 0, 0);
    double radius = 1.0;
};

struct QuadricData {
    double A = 0, B = 0, C = 0, D = 0, E = 0, F = 0;
    double G = 0, H = 0, I = 0, J = 0;
};

struct CylinderConeData {
    Vec3 base = Vec3(0, 0, 0);
    Vec3 axis = Vec3(0, 1, 0);
    double height = 1.0;
    double radius1 = 1.0;
    double radius2 = 1.0;
};

struct TriangleData {
    Vec3 v0 = Vec3(0, 0, 0);
    Vec3 v1 = Vec3(1, 0, 0);
    Vec3 v2 = Vec3(0, 1, 0);
};

// Object
struct Object {
    ObjectType type = SPHERE;
    int pigmentIdx = 0;
    int finishIdx = 0;
    
    // Union de dados
    SphereData sphere;
    std::vector<Plane> faces;
    QuadricData quadric;
    CylinderConeData cylinderCone;
    TriangleData triangle;
};

// Light
struct Light {
    Vec3 position;
    Vec3 color;
    Vec3 attenuation; // constant, linear, quadratic
    
    Light(const Vec3& pos = Vec3(), const Vec3& col = Vec3(1,1,1), 
          const Vec3& atten = Vec3(1,0,0))
        : position(pos), color(col), attenuation(atten) {}
};

// HitInfo
struct HitInfo {
    bool hit = false;
    double t = 1e10;
    Vec3 point;
    Vec3 normal;
    int objectIdx = -1;
};

// Scene
struct Scene {
    // Câmera
    Vec3 eye;
    Vec3 lookAt;
    Vec3 up;
    double fovy;
    
    // Elementos
    std::vector<Light> lights;
    std::vector<Pigment> pigments;
    std::vector<Finish> finishes;
    std::vector<Object> objects;
    
    Scene() : eye(0,0,0), lookAt(0,0,-1), up(0,1,0), fovy(40) {}
};

#endif