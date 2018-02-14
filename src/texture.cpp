#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(int width, int height)
    : id(0), filename("renderbuffer"), width(width), height(height) {
  glGenTextures(1, &this->id);
  glBindTexture(GL_TEXTURE_2D, this->id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);

  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(std::string filename) : id(0), filename(filename) {
  int texChannels;
  stbi_set_flip_vertically_on_load(true);
  stbi_uc *pixels = stbi_load(filename.c_str(), &this->width, &this->height,
                              &texChannels, STBI_rgb_alpha);
  if (pixels != nullptr) {
    glGenTextures(1, &this->id);

    glBindTexture(GL_TEXTURE_2D, this->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
  } else {
    throw std::runtime_error("Invalid texture file");
  }
}

Texture::Texture(const std::vector<std::string> &textures) : id(0) {
  glGenTextures(1, &this->id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, this->id);
  int width, height, nrChannels;
  for (unsigned int i = 0; i < textures.size(); i++) {
    unsigned char *data =
        stbi_load(textures[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                   0, GL_RGB, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      stbi_image_free(data);
      throw std::runtime_error("Invalid cubemap texture file");
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Texture::~Texture() {
  if (this->id != 0) {
    glDeleteTextures(1, &this->id);
  }
}
