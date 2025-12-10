// include/shading.hpp
#ifndef SHADING_HPP
#define SHADING_HPP

#include "scene.hpp"

constexpr int MAX_DEPTH = 5;
constexpr double SHADOW_BIAS = 0.001;

Vec3 traceRay(const Ray& ray, const Scene& scene, int depth = 0);

#endif
