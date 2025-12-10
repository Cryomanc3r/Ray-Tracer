// src/main.cpp
#include "../include/raytracer.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

struct Config {
    std::string inputFile;
    std::string outputFile;
    int width = 800;
    int height = 600;
    int samples = 16;
    double aperture = 0.0;
    double focusDist = 10.0;
};

void printUsage(const char* programName) {
    std::cerr << "Uso: " << programName 
              << " <cena.in> <saida.ppm> [largura] [altura] [amostras] [abertura] [dist_focal]" 
              << std::endl;
    std::cerr << "Exemplo: " << programName 
              << " testes/test5.in resultados/output.ppm" << std::endl;
    std::cerr << "Padrão: 800x600, 16 amostras, sem DOF" << std::endl;
}

bool parseArgs(int argc, char** argv, Config& config) {
    if (argc < 3) {
        printUsage(argv[0]);
        return false;
    }
    
    config.inputFile = argv[1];
    config.outputFile = argv[2];
    
    if (argc >= 4) config.width = std::atoi(argv[3]);
    if (argc >= 5) config.height = std::atoi(argv[4]);
    if (argc >= 6) config.samples = std::atoi(argv[5]);
    if (argc >= 7) config.aperture = std::atof(argv[6]);
    if (argc >= 8) config.focusDist = std::atof(argv[7]);
    
    // Validação
    if (config.width <= 0) config.width = 800;
    if (config.height <= 0) config.height = 600;
    if (config.samples <= 0) config.samples = 1;
    if (config.aperture < 0) config.aperture = 0.0;
    if (config.focusDist <= 0) config.focusDist = 10.0;
    
    return true;
}

void printConfig(const Config& config) {
    std::cout << "=== Ray Tracer RT-1 ===" << std::endl;
    std::cout << "Cena: " << config.inputFile << std::endl;
    std::cout << "Saída: " << config.outputFile << std::endl;
    std::cout << "Resolução: " << config.width << "x" << config.height << std::endl;
    std::cout << "Amostras por pixel: " << config.samples << std::endl;
    if (config.aperture > 0) {
        std::cout << "Depth of Field: abertura=" << config.aperture 
                  << ", foco=" << config.focusDist << std::endl;
    }
    std::cout << "=======================" << std::endl;
}

int main(int argc, char** argv) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    Config config;
    if (!parseArgs(argc, argv, config)) {
        return 1;
    }
    
    printConfig(config);
    
    // Criar e configurar ray tracer
    RayTracer tracer(config.width, config.height, config.samples);
    tracer.setDOF(config.aperture, config.focusDist);
    
    // Carregar cena
    if (!tracer.loadScene(config.inputFile)) {
        std::cerr << "Erro ao carregar cena: " << config.inputFile << std::endl;
        return 1;
    }
    
    // Renderizar
    std::cout << "Renderizando cena..." << std::endl;
    tracer.render();
    
    // Salvar imagem
    std::cout << "Salvando imagem: " << config.outputFile << std::endl;
    if (!tracer.savePPM(config.outputFile)) {
        std::cerr << "Erro ao salvar imagem" << std::endl;
        return 1;
    }
    
    std::cout << "Concluído!" << std::endl;
    return 0;
}