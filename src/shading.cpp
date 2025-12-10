// src/shading.cpp
#include "../include/shading.hpp"
#include "../include/intersect.hpp"
#include "../include/pigment.hpp"
#include <cmath>
#include <algorithm>

namespace {

// Refração usando Lei de Snell
bool refract(const Vec3& incident, const Vec3& normal, double ior, Vec3& refracted) {
    double cosi = incident.dot(normal);
    double etai = 1.0, etat = ior;
    Vec3 n = normal;
    
    if (cosi < 0) {
        cosi = -cosi;
    } else {
        std::swap(etai, etat);
        n = -normal;
    }
    
    double eta = etai / etat;
    double k = 1.0 - eta * eta * (1.0 - cosi * cosi);
    
    if (k < 0) return false; // Reflexão total interna
    
    refracted = (incident * eta + n * (eta * cosi - std::sqrt(k))).normalize();
    return true;
}

// Calcular atenuação da luz
inline double calculateAttenuation(const Light& light, double distance) {
    return 1.0 / (light.attenuation.x + 
                  light.attenuation.y * distance + 
                  light.attenuation.z * distance * distance);
}

// Verificar se ponto está em sombra
bool isInShadow(const Vec3& point, const Vec3& normal, const Light& light, const Scene& scene) {
    Vec3 lightDir = (light.position - point).normalize();
    double lightDist = (light.position - point).length();
    
    Ray shadowRay(point + normal * SHADOW_BIAS, lightDir);
    HitInfo shadowHit = findClosestHit(shadowRay, scene);
    
    return shadowHit.hit && shadowHit.t < lightDist - SHADOW_BIAS;
}

// Componente ambiente
Vec3 calculateAmbient(const Vec3& baseColor, const Light& ambientLight, double ka) {
    return baseColor.mul(ambientLight.color) * ka;
}

// Componente difusa
Vec3 calculateDiffuse(const Vec3& baseColor, const Vec3& normal, const Vec3& lightDir,
                      const Light& light, double kd, double attenuation) {
    double diff = std::max(0.0, normal.dot(lightDir));
    return baseColor.mul(light.color) * (kd * diff * attenuation);
}

// Componente especular (Phong)
Vec3 calculateSpecular(const Vec3& normal, const Vec3& lightDir, const Vec3& viewDir,
                       const Light& light, double ks, double alpha, double attenuation) {
    Vec3 reflectDir = lightDir.reflect(normal);
    double spec = std::pow(std::max(0.0, viewDir.dot(reflectDir)), alpha);
    return light.color * (ks * spec * attenuation);
}

// Iluminação local (Phong)
Vec3 calculateLocalIllumination(const HitInfo& hit, const Scene& scene, const Ray& ray) {
    const Object& obj = scene.objects[hit.objectIdx];
    const Pigment& pigment = scene.pigments[obj.pigmentIdx];
    const Finish& finish = scene.finishes[obj.finishIdx];
    
    Vec3 baseColor = getPigmentColor(pigment, hit.point);
    Vec3 viewDir = (ray.origin - hit.point).normalize();
    Vec3 color(0, 0, 0);
    
    // Luz ambiente (primeira luz)
    if (!scene.lights.empty()) {
        color = calculateAmbient(baseColor, scene.lights[0], finish.ka);
    }
    
    // Luzes pontuais (restantes)
    for (size_t i = 1; i < scene.lights.size(); i++) {
        const Light& light = scene.lights[i];
        
        if (isInShadow(hit.point, hit.normal, light, scene)) continue;
        
        Vec3 lightDir = (light.position - hit.point).normalize();
        double lightDist = (light.position - hit.point).length();
        double atten = calculateAttenuation(light, lightDist);
        
        color = color + calculateDiffuse(baseColor, hit.normal, lightDir, light, finish.kd, atten);
        color = color + calculateSpecular(hit.normal, lightDir, viewDir, light, finish.ks, finish.alpha, atten);
    }
    
    return color.clamp();
}

// Calcular reflexão
Vec3 calculateReflection(const HitInfo& hit, const Scene& scene, const Ray& ray, 
                         const Finish& finish, int depth) {
    if (finish.kr <= 0 || depth >= MAX_DEPTH) return Vec3(0, 0, 0);
    
    Vec3 reflectDir = ray.direction.reflect(hit.normal);
    Ray reflectRay(hit.point + hit.normal * SHADOW_BIAS, reflectDir);
    
    return traceRay(reflectRay, scene, depth + 1) * finish.kr;
}

// Calcular refração
Vec3 calculateRefraction(const HitInfo& hit, const Scene& scene, const Ray& ray,
                         const Finish& finish, int depth) {
    if (finish.kt <= 0 || depth >= MAX_DEPTH) return Vec3(0, 0, 0);
    
    Vec3 refractDir;
    if (!refract(ray.direction, hit.normal, finish.ior, refractDir)) {
        return Vec3(0, 0, 0);
    }
    
    Ray refractRay(hit.point - hit.normal * SHADOW_BIAS, refractDir);
    return traceRay(refractRay, scene, depth + 1) * finish.kt;
}

// Shader completo
Vec3 shade(const HitInfo& hit, const Scene& scene, const Ray& ray, int depth) {
    const Finish& finish = scene.finishes[scene.objects[hit.objectIdx].finishIdx];
    
    Vec3 color = calculateLocalIllumination(hit, scene, ray);
    color = color + calculateReflection(hit, scene, ray, finish, depth);
    color = color + calculateRefraction(hit, scene, ray, finish, depth);
    
    return color.clamp();
}

} // namespace anônimo

// Função principal de ray tracing
Vec3 traceRay(const Ray& ray, const Scene& scene, int depth) {
    if (depth > MAX_DEPTH) return Vec3(0, 0, 0);
    
    HitInfo hit = findClosestHit(ray, scene);
    
    if (hit.hit) {
        return shade(hit, scene, ray, depth);
    }
    
    // Cor de fundo
    return Vec3(0.1, 0.1, 0.1);
}