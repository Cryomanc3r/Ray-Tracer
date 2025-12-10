// src/pigment.cpp
#include "../include/pigment.hpp"
#include <cmath>

Vec3 getPigmentColor(const Pigment& pigment, const Vec3& point) {
    switch (pigment.type) {
        case SOLID:
            return pigment.color1;
            
        case CHECKER: {
            int xi = static_cast<int>(std::floor(point.x / pigment.scale));
            int yi = static_cast<int>(std::floor(point.y / pigment.scale));
            int zi = static_cast<int>(std::floor(point.z / pigment.scale));
            
            bool even = ((xi + yi + zi) & 1) == 0;
            return even ? pigment.color1 : pigment.color2;
        }
        
        case TEXMAP: {
            if (pigment.textureData.empty()) return Vec3(1, 1, 1);
            
            double px = point.x, py = point.y, pz = point.z, pw = 1.0;
            
            double s = pigment.p0[0]*px + pigment.p0[1]*py + pigment.p0[2]*pz + pigment.p0[3]*pw;
            double t = pigment.p1[0]*px + pigment.p1[1]*py + pigment.p1[2]*pz + pigment.p1[3]*pw;
            
            s = s - std::floor(s);
            t = t - std::floor(t);
            
            int u = static_cast<int>(s * pigment.textureWidth) % pigment.textureWidth;
            int v = static_cast<int>(t * pigment.textureHeight) % pigment.textureHeight;
            
            if (u < 0) u += pigment.textureWidth;
            if (v < 0) v += pigment.textureHeight;
            
            int idx = v * pigment.textureWidth + u;
            if (idx >= 0 && idx < static_cast<int>(pigment.textureData.size())) {
                return pigment.textureData[idx];
            }
            break;
        }
    }
    
    return Vec3(1, 1, 1);
}