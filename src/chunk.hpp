#pragma once
#define CHUNK_SIZE 16
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <iostream>
#include <map>

enum class Material : unsigned char { Air, Dirt, Sand };

struct Block {
  enum Material material;
};

class Chunk {
 public:
  Chunk(void);
  Chunk(Chunk const& src);
  ~Chunk(void);
  Chunk& operator=(Chunk const& rhs);

  Block data[65536] = {};
  glm::ivec3 pos;

  inline Block get_block(glm::ivec3 index);
  inline void set_block(Block block, glm::ivec3 index);
};

class ChunkManager {
 public:
  ChunkManager(void);
  ChunkManager(ChunkManager const& src);
  ~ChunkManager(void);
  ChunkManager& operator=(ChunkManager const& rhs);

  void update(glm::vec3 player_pos);
  void setRenderDistance(unsigned char renderDistance);

 private:
  unsigned char _renderDistance;
  std::map<glm::ivec2, Chunk> _chunks;
};
