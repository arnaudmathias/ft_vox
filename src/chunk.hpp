#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <queue>
#include <unordered_map>
#include <vector>
#include "culling.hpp"
#include "generator.hpp"
#include "io.hpp"
#include "meshing.hpp"
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

class Chunk {
 public:
  Chunk(glm::ivec3 pos);
  Chunk(Chunk const& src);
  ~Chunk(void);
  Chunk& operator=(Chunk const& rhs);

  Block data[65536] = {};
  glm::vec3 aabb_center;
  glm::vec3 aabb_halfsize;

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
  ChunkManager(uint32_t seed);
  ChunkManager(ChunkManager const& src);
  ~ChunkManager(void);
  ChunkManager& operator=(ChunkManager const& rhs);

  void update(const glm::vec3& player_pos);
  void setRenderAttributes(Renderer& renderer, glm::vec3 player_pos);
  void setRenderDistance(unsigned char renderDistance);

 private:
  void addChunkToQueue(glm::ivec2 chunk_pos);
  void loadChunks();
  void unloadChunks(glm::ivec2 current_chunk_pos);
  std::string getRegionFilename(glm::ivec2 pos);
  unsigned char _renderDistance;
  std::unordered_map<glm::ivec2, Chunk, ivec2Comparator> _chunks;
  std::deque<glm::ivec2> to_load;  // FIFO Queue
  FrustrumCulling frustrum_culling;
  uint32_t _seed;
};
