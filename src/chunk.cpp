#include "chunk.hpp"

Chunk::Chunk(void) {
  for (int y = 0; y < 256; y++) {
    for (int x = 0; x < 16; x++) {
      for (int z = 0; z < 16; z++) {
        set_block({Material::Dirt}, glm::ivec3(x, y, z));
      }
    }
  }
}

Chunk::Chunk(Chunk const& src) { *this = src; }

Chunk::~Chunk(void) {}

Chunk& Chunk::operator=(Chunk const& rhs) {
  if (this != &rhs) {
    this->pos = rhs.pos;
  }
  return (*this);
}

inline Block Chunk::get_block(glm::ivec3 index) {
  return (this->data[index.y * CHUNK_SIZE * CHUNK_SIZE + index.x * CHUNK_SIZE +
                     index.z]);
}

inline void Chunk::set_block(Block block, glm::ivec3 index) {
  this->data[index.y * CHUNK_SIZE * CHUNK_SIZE + index.x * CHUNK_SIZE +
             index.z] = block;
}

ChunkManager::ChunkManager(void) : _renderDistance(4) {
  glm::ivec2 pos(0);
  for (int x = -this->_renderDistance; x < this->_renderDistance; x++) {
    for (int z = -this->_renderDistance; z < this->_renderDistance; z++) {
      // glm::ivec2 chunk_pos((pos.x + x) % CHUNK_SIZE, (pos.y + z) %
      // CHUNK_SIZE);  auto chunk = _chunks.find(pos);
      // if (chunk == _chunks.end()) {
      //}
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

void ChunkManager::update(glm::vec3 player_pos) {
  glm::ivec3 pos(player_pos);
  for (int x = -this->_renderDistance; x < this->_renderDistance; x++) {
    for (int z = -this->_renderDistance; z < this->_renderDistance; z++) {
      // chunks.find
    }
  }
}

void ChunkManager::setRenderDistance(unsigned char rd) {
  this->_renderDistance = rd;
}
