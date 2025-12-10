// src/loader.cpp
#include "../include/loader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

// Pular comentários em arquivo PPM
static void skipComments(std::ifstream& file) {
    char c;
    file.get(c);
    while (file && (c == '#' || c == '\n')) {
        if (c == '#') {
            while (file.get(c) && c != '\n');
        }
        if (!file.get(c)) break;
    }
    if (file) file.unget();
}

// Carregar textura PPM
bool loadPPM(const std::string& filename, Pigment& pigment) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir textura: " << filename << std::endl;
        return false;
    }
    
    std::string magic;
    file >> magic;
    
    if (magic != "P3" && magic != "P6") {
        std::cerr << "Formato PPM não suportado: " << magic << std::endl;
        return false;
    }
    
    skipComments(file);
    
    int maxval;
    file >> pigment.textureWidth >> pigment.textureHeight >> maxval;
    
    pigment.textureData.resize(pigment.textureWidth * pigment.textureHeight);
    
    if (magic == "P3") {
        // ASCII
        for (auto& pixel : pigment.textureData) {
            int r, g, b;
            file >> r >> g >> b;
            pixel = Vec3(r, g, b) / maxval;
        }
    } else {
        // Binário
        file.get(); // Pular newline
        for (auto& pixel : pigment.textureData) {
            unsigned char rgb[3];
            file.read(reinterpret_cast<char*>(rgb), 3);
            pixel = Vec3(rgb[0], rgb[1], rgb[2]) / maxval;
        }
    }
    
    file.close();
    return true;
}

// Template para ler dados genéricos
template<typename T>
static bool readData(std::ifstream& file, T& data) {
    return static_cast<bool>(file >> data);
}

// Carregar pigmento
static bool loadPigment(std::ifstream& file, Pigment& pigment) {
    std::string type;
    if (!(file >> type)) return false;
    
    if (type == "solid") {
        pigment.type = SOLID;
        return readData(file, pigment.color1.x) &&
               readData(file, pigment.color1.y) &&
               readData(file, pigment.color1.z);
    } 
    else if (type == "checker") {
        pigment.type = CHECKER;
        return readData(file, pigment.color1.x) &&
               readData(file, pigment.color1.y) &&
               readData(file, pigment.color1.z) &&
               readData(file, pigment.color2.x) &&
               readData(file, pigment.color2.y) &&
               readData(file, pigment.color2.z) &&
               readData(file, pigment.scale);
    } 
    else if (type == "texmap") {
        pigment.type = TEXMAP;
        if (!(file >> pigment.texturePath)) return false;
        for (int i = 0; i < 4; i++) {
            if (!readData(file, pigment.p0[i])) return false;
        }
        for (int i = 0; i < 4; i++) {
            if (!readData(file, pigment.p1[i])) return false;
        }
        return loadPPM(pigment.texturePath, pigment);
    }
    
    return false;
}

// Carregar finish
static bool loadFinish(std::ifstream& file, Finish& finish) {
    return readData(file, finish.ka) &&
           readData(file, finish.kd) &&
           readData(file, finish.ks) &&
           readData(file, finish.alpha) &&
           readData(file, finish.kr) &&
           readData(file, finish.kt) &&
           readData(file, finish.ior);
}

// Carregar objeto
static bool loadObject(std::ifstream& file, Object& obj) {
    int pigmentIdx, finishIdx;
    if (!(file >> pigmentIdx >> finishIdx)) return false;
    
    obj.pigmentIdx = pigmentIdx;
    obj.finishIdx = finishIdx;
    
    std::string type;
    if (!(file >> type)) return false;
    
    if (type == "sphere") {
        obj.type = SPHERE;
        return readData(file, obj.sphere.center.x) &&
               readData(file, obj.sphere.center.y) &&
               readData(file, obj.sphere.center.z) &&
               readData(file, obj.sphere.radius);
    }
    else if (type == "polyhedron") {
        obj.type = POLYHEDRON;
        int numFaces;
        if (!(file >> numFaces)) return false;
        
        obj.faces.reserve(numFaces);
        for (int i = 0; i < numFaces; i++) {
            double a, b, c, d;
            if (!(readData(file, a) && readData(file, b) && 
                  readData(file, c) && readData(file, d))) return false;
            obj.faces.emplace_back(a, b, c, d);
        }
        return true;
    }
    else if (type == "triangle") {
        obj.type = TRIANGLE;
        return readData(file, obj.triangle.v0.x) &&
               readData(file, obj.triangle.v0.y) &&
               readData(file, obj.triangle.v0.z) &&
               readData(file, obj.triangle.v1.x) &&
               readData(file, obj.triangle.v1.y) &&
               readData(file, obj.triangle.v1.z) &&
               readData(file, obj.triangle.v2.x) &&
               readData(file, obj.triangle.v2.y) &&
               readData(file, obj.triangle.v2.z);
    }
    else if (type == "cylinder") {
        obj.type = CYLINDER;
        return readData(file, obj.cylinderCone.base.x) &&
               readData(file, obj.cylinderCone.base.y) &&
               readData(file, obj.cylinderCone.base.z) &&
               readData(file, obj.cylinderCone.axis.x) &&
               readData(file, obj.cylinderCone.axis.y) &&
               readData(file, obj.cylinderCone.axis.z) &&
               readData(file, obj.cylinderCone.height) &&
               readData(file, obj.cylinderCone.radius1);
    }
    else if (type == "cone") {
        obj.type = CONE;
        return readData(file, obj.cylinderCone.base.x) &&
               readData(file, obj.cylinderCone.base.y) &&
               readData(file, obj.cylinderCone.base.z) &&
               readData(file, obj.cylinderCone.axis.x) &&
               readData(file, obj.cylinderCone.axis.y) &&
               readData(file, obj.cylinderCone.axis.z) &&
               readData(file, obj.cylinderCone.height) &&
               readData(file, obj.cylinderCone.radius1);
    }
    else if (type == "quadric") {
        obj.type = QUADRIC;
        return readData(file, obj.quadric.A) &&
               readData(file, obj.quadric.B) &&
               readData(file, obj.quadric.C) &&
               readData(file, obj.quadric.D) &&
               readData(file, obj.quadric.E) &&
               readData(file, obj.quadric.F) &&
               readData(file, obj.quadric.G) &&
               readData(file, obj.quadric.H) &&
               readData(file, obj.quadric.I) &&
               readData(file, obj.quadric.J);
    }
    
    return false;
}

// Carregar cena completa
bool loadScene(const std::string& filename, Scene& scene) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir cena: " << filename << std::endl;
        return false;
    }
    
    // 1. Câmera
    if (!(file >> scene.eye.x >> scene.eye.y >> scene.eye.z)) return false;
    if (!(file >> scene.lookAt.x >> scene.lookAt.y >> scene.lookAt.z)) return false;
    if (!(file >> scene.up.x >> scene.up.y >> scene.up.z)) return false;
    if (!(file >> scene.fovy)) return false;
    
    // 2. Luzes
    int numLights;
    if (!(file >> numLights)) return false;
    scene.lights.reserve(numLights);
    
    for (int i = 0; i < numLights; i++) {
        Light light;
        if (!(file >> light.position.x >> light.position.y >> light.position.z)) return false;
        if (!(file >> light.color.x >> light.color.y >> light.color.z)) return false;
        if (!(file >> light.attenuation.x >> light.attenuation.y >> light.attenuation.z)) return false;
        scene.lights.push_back(light);
    }
    
    // 3. Pigmentos
    int numPigments;
    if (!(file >> numPigments)) return false;
    scene.pigments.reserve(numPigments);
    
    for (int i = 0; i < numPigments; i++) {
        Pigment pigment;
        if (!loadPigment(file, pigment)) return false;
        scene.pigments.push_back(pigment);
    }
    
    // 4. Finishes
    int numFinishes;
    if (!(file >> numFinishes)) return false;
    scene.finishes.reserve(numFinishes);
    
    for (int i = 0; i < numFinishes; i++) {
        Finish finish;
        if (!loadFinish(file, finish)) return false;
        scene.finishes.push_back(finish);
    }
    
    // 5. Objetos
    int numObjects;
    if (!(file >> numObjects)) return false;
    scene.objects.reserve(numObjects);
    
    for (int i = 0; i < numObjects; i++) {
        Object obj;
        if (!loadObject(file, obj)) return false;
        scene.objects.push_back(obj);
    }
    
    file.close();
    return true;
}