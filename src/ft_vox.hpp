#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 256
#define MODEL_HEIGHT 16
#define MODEL_PER_CHUNK CHUNK_HEIGHT / MODEL_HEIGHT

enum class BlockSide : unsigned int { Front, Back, Left, Right, Bottom, Up };

enum class Material : unsigned char { Air, Stone, Dirt, Sand };

struct Block {
  enum Material material;
  bool operator==(const Block& rhs) const { return material == rhs.material; };
  bool operator!=(const Block& rhs) const { return material != rhs.material; };
};

struct Texture_lookup {
  int side[6];
};

struct Vertex {
  glm::vec4 position = {0, 0, 0, 0};
  float texture_id = {0};
  Vertex() : position({0.0f, 0.0f, 0.0f, 0.0f}){};
  Vertex(Vertex const& src) { *this = src; }
  Vertex& operator=(Vertex const& rhs) {
    if (this != &rhs) {
      this->position = rhs.position;
      this->texture_id = rhs.texture_id;
    }
    return (*this);
  };
};
