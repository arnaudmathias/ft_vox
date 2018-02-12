#include "chunk.hpp"

std::vector<glm::vec3> plane_vertices = {
    {-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},   {1.0f, -1.0f, 0.0f},
    {1.0f, -1.0f, 0.0f}, {-1.0f, -1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}};

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

void Chunk::mesh() {
  std::vector<glm::vec3> vertices;
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      for (int z = 0; z < CHUNK_SIZE / 2; z++) {
        Block front_block = get_block({x, y, z});
        if (front_block.material != Material::Air) {
          vertices.insert(vertices.end(), plane_vertices.begin(),
                          plane_vertices.end());
          break;
        }
      }
    }
  }
  std::cout << "Mesher: " << vertices.size() << " vertices" << std::endl;
};

inline Block Chunk::get_block(glm::ivec3 index) {
  return (this->data[index.y * CHUNK_SIZE * CHUNK_SIZE + index.x * CHUNK_SIZE +
                     index.z]);
}

inline void Chunk::set_block(Block block, glm::ivec3 index) {
  this->data[index.y * CHUNK_SIZE * CHUNK_SIZE + index.x * CHUNK_SIZE +
             index.z] = block;
}

ChunkManager::ChunkManager(void) : _renderDistance(3) {
  glm::ivec2 pos(0);
  for (int x = -this->_renderDistance; x <= this->_renderDistance; x++) {
    for (int z = -this->_renderDistance; z <= this->_renderDistance; z++) {
      glm::ivec2 chunk_pos(pos.x + x * CHUNK_SIZE, pos.y + z * CHUNK_SIZE);
      auto chunk = _chunks.find(chunk_pos);
      if (chunk == _chunks.end()) {
        Chunk newChunk;
        newChunk.mesh();
        _chunks.insert(std::make_pair(chunk_pos, newChunk));
      }
    }
  }
  std::cout << "Chunk manager load: " << _chunks.size() << " chunks"
            << std::endl;
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
