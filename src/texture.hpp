#pragma once
#include <vector>
#include "env.hpp"

struct Texture {
  Texture(int width, int height);                     // RenderBuffer
  Texture(std::string filename);                      // Basic texture
  Texture(const std::vector<std::string> &textures);  // Cubemap
  ~Texture();
  std::string filename;
  GLuint id;
  int height;
  int width;
};
