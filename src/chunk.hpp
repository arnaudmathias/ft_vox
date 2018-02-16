#pragma once
#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 256
#define MODEL_HEIGHT 16
#define GLM_ENABLE_EXPERIMENTAL
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <queue>
#include <unordered_map>
#include <vector>
#include "generator.hpp"
#include "renderer.hpp"
#include "vao.hpp"

struct ivec2Comparator {
  size_t operator()(const glm::ivec2& k) const {
    size_t h = std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1);
    return (h);
  }

  bool operator()(const glm::ivec2& a, const glm::ivec2& b) const {
    return (a.x == b.x && a.y == b.y);
  }
};

enum class BlockSide : unsigned int { Front, Back, Left, Right, Bottom, Up };

enum class Material : unsigned char { Air, Stone, Dirt, Sand };

struct Texture_lookup {
  int side[6];
};

struct Block {
  enum Material material;
  bool operator==(const Block& rhs) const { return material == rhs.material; };
  bool operator!=(const Block& rhs) const { return material != rhs.material; };
};

class Chunk {
 public:
  Chunk(glm::ivec3 pos);
  Chunk(Chunk const& src);
  ~Chunk(void);
  Chunk& operator=(Chunk const& rhs);

  Block data[65536] = {};

  void mesh();

  inline Block get_block(glm::ivec3 index);
  inline void set_block(Block block, glm::ivec3 index);
  const RenderAttrib& getRenderAttrib();
  glm::ivec3 get_pos();

 private:
  Chunk(void);
  bool _dirty[CHUNK_HEIGHT / MODEL_HEIGHT] = {true};  // is Remesh needed ?
  RenderAttrib _renderAttrib;
  glm::ivec3 _pos;
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
  void addChunkToQueue(glm::ivec2 chunk_pos);
  void loadChunks();
  void unloadChunks(glm::ivec2 current_chunk_pos);
  unsigned char _renderDistance;
  std::unordered_map<glm::ivec2, Chunk, ivec2Comparator> _chunks;
  std::deque<glm::ivec2> to_load;  // FIFO Queue
};
