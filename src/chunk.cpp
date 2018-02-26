#include "chunk.hpp"

Chunk::Chunk() : Chunk(glm::ivec3(0)) {}

Chunk::Chunk(glm::ivec3 pos)
    : aabb_center(0.0f), aabb_halfsize(0.0f), _pos(pos) {
  _renderAttrib.model = glm::translate(_pos);
  generator::generate_chunk(this->data, glm::vec3(_pos));
  for (int i = 0; i < MODEL_PER_CHUNK; i++) {
    this->_dirty[i] = true;
  }
}

Chunk::Chunk(Chunk const& src) { *this = src; }

Chunk::~Chunk(void) {
  for (auto& vao : _renderAttrib.vaos) {
    delete vao;
  }
}

Chunk& Chunk::operator=(Chunk const& rhs) {
  if (this != &rhs) {
    this->_pos = rhs._pos;
    std::memcpy(this->data, rhs.data, sizeof(this->data));
    this->aabb_center = rhs.aabb_center;
    this->aabb_halfsize = rhs.aabb_halfsize;
    this->_renderAttrib.vaos = rhs._renderAttrib.vaos;
    this->_renderAttrib.model = rhs._renderAttrib.model;
    std::memcpy(this->_dirty, rhs._dirty, sizeof(this->_dirty));
    this->_renderAttrib = rhs._renderAttrib;
    this->_pos = rhs._pos;
  }
  return (*this);
}

void Chunk::mesh() {
  mesher::greedy(data, _dirty, _renderAttrib);
  mesher::get_aabb(data, aabb_center, aabb_halfsize, _pos);
};

const RenderAttrib& Chunk::getRenderAttrib() { return (this->_renderAttrib); }

inline Block Chunk::get_block(glm::ivec3 index) {
  if (index.x < 0 || index.x >= CHUNK_SIZE || index.y < 0 || index.y >= 256 ||
      index.z < 0 || index.z >= CHUNK_SIZE) {
    Block block = {};
    return (block);
  }
  return (this->data[index.y * CHUNK_SIZE * CHUNK_SIZE + index.x * CHUNK_SIZE +
		     index.z]);
}

inline void Chunk::set_block(Block block, glm::ivec3 index) {
  this->data[index.y * CHUNK_SIZE * CHUNK_SIZE + index.x * CHUNK_SIZE +
	     index.z] = block;
  this->_dirty[index.y / MODEL_HEIGHT] = true;
}
glm::ivec3 Chunk::get_pos() { return (_pos); }

ChunkManager::ChunkManager(void) : _renderDistance(10) {
  glm::ivec2 pos(0);
  for (int x = -this->_renderDistance; x <= this->_renderDistance; x++) {
    for (int z = -this->_renderDistance; z <= this->_renderDistance; z++) {
      glm::ivec2 chunk_pos(pos.x + x * CHUNK_SIZE, pos.y + z * CHUNK_SIZE);
      addChunkToQueue(pos);
    }
  }
}

ChunkManager::ChunkManager(ChunkManager const& src) { *this = src; }

ChunkManager::~ChunkManager(void) {}

ChunkManager& ChunkManager::operator=(ChunkManager const& rhs) {
  if (this != &rhs) {
    this->_renderDistance = rhs._renderDistance;
  }
  return (*this);
}

void ChunkManager::update(const glm::vec3& player_pos) {
  glm::ivec2 pos =
      glm::ivec2((static_cast<int>(round(player_pos.x)) -
		  (static_cast<int>(round(player_pos.x)) % CHUNK_SIZE)),
		 (static_cast<int>(round(player_pos.z)) -
		  (static_cast<int>(round(player_pos.z)) % CHUNK_SIZE)));
  for (int x = -this->_renderDistance; x <= this->_renderDistance; x++) {
    for (int z = -this->_renderDistance; z <= this->_renderDistance; z++) {
      glm::ivec2 chunk_pos(pos.x + x * CHUNK_SIZE, pos.y + z * CHUNK_SIZE);
      addChunkToQueue(chunk_pos);
    }
  }
  unloadChunks(pos);
  loadChunks();
}

void ChunkManager::addChunkToQueue(glm::ivec2 chunk_pos) {
  auto chunk = _chunks.find(chunk_pos);
  if (chunk == _chunks.end()) {
    bool found = false;
    for (int i = 0; i < to_load.size(); i++) {
      if (to_load[i] == chunk_pos) {
	found = true;
      }
    }
    if (found == false) {
      to_load.push_back(chunk_pos);
    }
  }
}

void ChunkManager::loadChunks() {
  if (to_load.size() > 0) {
    int chunksPerFrame = 1;
    for (int i = 0; i < chunksPerFrame; i++) {
      if (to_load.size() == 0) break;
      glm::ivec2 chunk_position = to_load.front();
      _chunks.emplace(chunk_position,
		      Chunk({chunk_position.x, 0, chunk_position.y}));
      auto newchunk = _chunks.find(chunk_position);
      newchunk->second.mesh();
      to_load.pop_front();
    }
  }
}

void ChunkManager::unloadChunks(glm::ivec2 current_chunk_pos) {
  auto chunk_it = _chunks.begin();
  while (chunk_it != _chunks.end()) {
    auto old = chunk_it;
    chunk_it++;
    glm::ivec3 c_pos = old->second.get_pos();
    float dist = glm::distance(glm::vec2(c_pos.x, c_pos.z),
			       glm::vec2(current_chunk_pos));
    if (round(dist) / CHUNK_SIZE >
	static_cast<float>(this->_renderDistance) + 5) {
      _chunks.erase(old);
    }
  }
}

void ChunkManager::setRenderAttributes(Renderer& renderer,
				       glm::vec3 player_pos) {
  glm::ivec2 pos =
      glm::ivec2((static_cast<int>(round(player_pos.x)) -
		  (static_cast<int>(round(player_pos.x)) % CHUNK_SIZE)),
		 (static_cast<int>(round(player_pos.z)) -
		  (static_cast<int>(round(player_pos.z)) % CHUNK_SIZE)));
  frustrum_culling.updateViewPlanes(renderer.uniforms.view_proj);

  auto chunk_it = _chunks.begin();
  while (chunk_it != _chunks.end()) {
    glm::ivec3 c_pos = chunk_it->second.get_pos();
    float dist = glm::distance(glm::vec2(c_pos.x, c_pos.z), glm::vec2(pos));
    if (round(dist) / CHUNK_SIZE <
	static_cast<float>(this->_renderDistance + 1)) {
      if (frustrum_culling.cull(chunk_it->second.aabb_center,
				chunk_it->second.aabb_halfsize)) {
	renderer.addRenderAttrib(chunk_it->second.getRenderAttrib());
      }
    }
    chunk_it++;
  }
}

void ChunkManager::setRenderDistance(unsigned char rd) {
  this->_renderDistance = rd;
}
