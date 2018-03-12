#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
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

enum class MeshingType : unsigned int { Culling, Greedy };

class Chunk {
 public:
  Chunk(glm::ivec3 pos);
  Chunk(Chunk const& src);
  ~Chunk(void);
  Chunk& operator=(Chunk const& rhs);

  Block data[65536] = {};
  Biome biome_data[CHUNK_SIZE * CHUNK_SIZE] = {};
  glm::vec3 aabb_center;
  glm::vec3 aabb_halfsize;
  bool dirty[CHUNK_HEIGHT / MODEL_HEIGHT] = {true};  // is Remesh needed ?

  void mesh(enum MeshingType meshing_type);
  void generate();

  inline Block get_block(glm::ivec3 index);
  inline Biome get_biome(glm::ivec3 index);
  inline void set_block(Block block, glm::ivec3 index);
  const RenderAttrib& getRenderAttrib();
  glm::ivec3 get_pos();
  bool generated;  // Needed on unload to avoid writing empty chunk to disk
  void forceFullRemesh();
  void setDirty(int model_id);

 private:
  Chunk(void);
  RenderAttrib _renderAttrib;
  glm::ivec3 _pos;
  bool is_dirty();
};

class ChunkManager {
 public:
  ChunkManager(void);
  ChunkManager(uint32_t seed);
  ChunkManager(ChunkManager const& src);
  ~ChunkManager(void);
  ChunkManager& operator=(ChunkManager const& rhs);

  void update(const glm::vec3& player_pos);
  struct HitInfo rayCast(glm::vec3 ray_dir, glm::vec3 ray_pos, float max_dist);
  void setRenderAttributes(Renderer& renderer, glm::vec3 player_pos);
  void setRenderDistance(unsigned char renderDistance);
  void print_chunkmanager_info(Renderer& renderer, float window_height,
                               float window_width);
  void increaseRenderDistance();
  void decreaseRenderDistance();
  void setMeshingType(enum MeshingType type);
  void setBlockType(struct Block type);
  void reloadMesh();
  void set_block(Block block, glm::ivec3 index);
  void point_exploding(glm::ivec3 index, float intensity);
  void Draw_earth();
  void add_block(glm::ivec3 index);

 private:
  inline Block get_block(glm::ivec3 index);
  void addRegionToQueue(glm::ivec2 chunk_pos);
  void loadRegion(glm::ivec2 region_pos);
  void unloadRegion(glm::ivec2 region_pos);
  void unloadRegions(glm::ivec2 current_chunk_pos);
  std::string getRegionFilename(glm::ivec2 pos);
  void eraseUnloadedChunk(glm::ivec2 pos);
  unsigned char _renderDistance;
  std::unordered_map<glm::ivec2, Chunk, ivec2Comparator> _chunks;
  std::deque<glm::ivec2>
      to_update;  // User modified chunks, priority over everything else
  std::deque<glm::ivec2> to_mesh;
  std::deque<glm::ivec2> to_generate;
  std::deque<glm::ivec2> to_unload;
  FrustrumCulling frustrum_culling;
  uint32_t _seed;
  size_t _debug_chunks_rendered;
  enum MeshingType _meshing_type;
  struct Block _current_block;
};
