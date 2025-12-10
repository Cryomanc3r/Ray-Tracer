// src/raytracer.cpp
#include "../include/raytracer.hpp"
#include "../include/shading.hpp"
#include "../include/loader.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <algorithm>

RayTracer::RayTracer(int w, int h, int samples) 
    : width(w), height(h), samples(samples), aperture(0.0), focusDist(10.0) {
    frameBuffer.resize(width * height * 3);
}

bool RayTracer::loadScene(const std::string& filename) {
    return ::loadScene(filename, scene);
}

RayTracer::CameraParams RayTracer::setupCamera() const {
    CameraParams cam;
    
    cam.w = (scene.eye - scene.lookAt).normalize();
    cam.u = scene.up.cross(cam.w).normalize();
    cam.v = cam.w.cross(cam.u).normalize();
    
    cam.aspectRatio = static_cast<double>(width) / height;
    
    double fovyRad = scene.fovy * M_PI / 180.0;
    cam.viewportHeight = 2.0 * std::tan(fovyRad / 2.0);
    cam.viewportWidth = cam.viewportHeight * cam.aspectRatio;
    
    return cam;
}

Ray RayTracer::generateRay(int x, int y, double jitterX, double jitterY, 
                           const CameraParams& cam) const {
    double ndcX = (2.0 * (x + jitterX) / width) - 1.0;
    double ndcY = 1.0 - (2.0 * (y + jitterY) / height);
    
    Vec3 rayDir = cam.u * (ndcX * cam.viewportWidth / 2.0) +
                  cam.v * (ndcY * cam.viewportHeight / 2.0) - cam.w;
    rayDir = rayDir.normalize();
    
    Vec3 rayOrigin = scene.eye;
    
    // Depth of Field
    if (aperture > 0.0) {
        double dx, dy;
        do {
            dx = (static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0;
            dy = (static_cast<double>(rand()) / RAND_MAX) * 2.0 - 1.0;
        } while (dx * dx + dy * dy > 1.0);
        
        Vec3 offset = cam.u * (dx * aperture) + cam.v * (dy * aperture);
        rayOrigin = scene.eye + offset;
        
        Vec3 focusPoint = scene.eye + rayDir * focusDist;
        rayDir = (focusPoint - rayOrigin).normalize();
    }
    
    return Ray(rayOrigin, rayDir);
}

void RayTracer::render() {
    CameraParams cam = setupCamera();
    
    std::cout << "Renderizando " << width << "x" << height 
              << " com " << samples << " amostras..." << std::endl;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Vec3 pixelColor(0, 0, 0);
            
            for (int s = 0; s < samples; s++) {
                double jitterX = static_cast<double>(rand()) / RAND_MAX;
                double jitterY = static_cast<double>(rand()) / RAND_MAX;
                
                Ray ray = generateRay(x, y, jitterX, jitterY, cam);
                pixelColor = pixelColor + traceRay(ray, scene);
            }
            
            pixelColor = pixelColor / static_cast<double>(samples);
            
            // Converter para RGB [0-255] com clamping
            int idx = (y * width + x) * 3;
            frameBuffer[idx + 0] = static_cast<unsigned char>(
                std::clamp(pixelColor.x * 255.0, 0.0, 255.0));
            frameBuffer[idx + 1] = static_cast<unsigned char>(
                std::clamp(pixelColor.y * 255.0, 0.0, 255.0));
            frameBuffer[idx + 2] = static_cast<unsigned char>(
                std::clamp(pixelColor.z * 255.0, 0.0, 255.0));
        }
        
        if (y % 10 == 0) {
            std::cout << "Progresso: " << (100 * y / height) << "%\r" << std::flush;
        }
    }
    
    std::cout << "Progresso: 100%" << std::endl;
}

bool RayTracer::savePPM(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao criar arquivo: " << filename << std::endl;
        return false;
    }
    
    file << "P3\n" << width << " " << height << "\n255\n";
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = (y * width + x) * 3;
            file << static_cast<int>(frameBuffer[idx + 0]) << " "
                 << static_cast<int>(frameBuffer[idx + 1]) << " "
                 << static_cast<int>(frameBuffer[idx + 2]) << " ";
        }
        file << "\n";
    }
    
    file.close();
    return true;
}