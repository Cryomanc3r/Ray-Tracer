// include/loader.hpp
#ifndef LOADER_HPP
#define LOADER_HPP

#include "scene.hpp"
#include <string>

bool loadPPM(const std::string& filename, Pigment& pigment);
bool loadScene(const std::string& filename, Scene& scene);

#endif