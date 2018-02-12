#pragma once
#define CHUNK_SIZE 16
#define MODEL_HEIGHT 16
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
//#include <glm/gtx/hash.hpp>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include "renderer.hpp"

struct ivec2Comparator {
  size_t operator()(const glm::ivec2& k) const {
    size_t h = std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1);
    return (h);
  }

  bool operator()(const glm::ivec2& a, const glm::ivec2& b) const {
    return (a.x == b.x && a.y == b.y);
  }
};

enum class Material : unsigned char { Air, Dirt, Sand };

struct Block {
  enum Material material;
  bool operator==(const Block& rhs) const { return material == rhs.material; };
  bool operator!=(const Block& rhs) const { return material != rhs.material; };
};

class Chunk {
 public:
  Chunk(void);
  Chunk(Chunk const& src);
  ~Chunk(void);
  Chunk& operator=(Chunk const& rhs);

  Block data[65536] = {};
  glm::ivec3 pos;

  void mesh();

  inline Block get_block(glm::ivec3 index);
  inline void set_block(Block block, glm::ivec3 index);
  const RenderAttrib& getRenderAttrib();

 private:
  RenderAttrib _renderAttrib;
};

class ChunkManager {
 public:
  ChunkManager(void);
  ChunkManager(ChunkManager const& src);
  ~ChunkManager(void);
  ChunkManager& operator=(ChunkManager const& rhs);

  void update(glm::vec3 player_pos);
  void setRenderAttributes(Renderer& renderer);
  void setRenderDistance(unsigned char renderDistance);

 private:
  unsigned char _renderDistance;
  std::unordered_map<glm::ivec2, Chunk, ivec2Comparator> _chunks;
};
