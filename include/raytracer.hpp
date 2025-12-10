// include/raytracer.hpp
#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include "scene.hpp"
#include <string>
#include <vector>

class RayTracer {
private:
    int width, height;
    int samples;
    double aperture;
    double focusDist;
    
    Scene scene;
    std::vector<unsigned char> frameBuffer;
    
    // Helpers
    struct CameraParams {
        Vec3 u, v, w;
        double aspectRatio;
        double viewportWidth;
        double viewportHeight;
    };
    
    CameraParams setupCamera() const;
    Ray generateRay(int x, int y, double jitterX, double jitterY, const CameraParams& cam) const;
    
public:
    RayTracer(int w = 800, int h = 600, int samples = 16);
    
    bool loadScene(const std::string& filename);
    void render();
    bool savePPM(const std::string& filename) const;
    
    void setSamples(int s) { samples = s; }
    void setDOF(double a, double f) { aperture = a; focusDist = f; }
};

#endif