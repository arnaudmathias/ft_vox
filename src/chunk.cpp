#include "chunk.hpp"

const std::vector<glm::vec3> cube_front = {
    {-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},   {0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}};

const std::vector<glm::vec3> cube_back = {
    {-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},   {0.5f, -0.5f, 0.5f},
    {0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}};

const std::vector<glm::vec3> cube_left = {
    {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},   {0.5f, 0.5f, -0.5f},
    {0.5f, 0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f}};

const std::vector<glm::vec3> cube_right = {
    {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f},   {-0.5f, 0.5f, -0.5f},
    {-0.5f, 0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}};

const std::vector<glm::vec3> cube_bottom = {
    {-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f},   {0.5f, -0.5f, -0.5f},
    {0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f}};

const std::vector<glm::vec3> cube_up = {
    {-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f},   {0.5f, 0.5f, -0.5f},
    {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, 0.5f}};

Chunk::Chunk() : Chunk(glm::ivec3(0)) {}

Chunk::Chunk(glm::ivec3 pos) : _pos(pos) {
  _renderAttrib.model = glm::translate(_pos);
  for (int y = 0; y < 256; y++) {
    for (int x = 0; x < 16; x++) {
      for (int z = 0; z < 16; z++) {
        if (y < 16 && x != 3 && y != 5)
          set_block({Material::Dirt}, glm::ivec3(x, y, z));
      }
    }
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
    this->_renderAttrib.vaos = rhs._renderAttrib.vaos;
    this->_renderAttrib.model = rhs._renderAttrib.model;
    std::memcpy(this->_dirty, rhs._dirty, sizeof(this->_dirty));
  }
  return (*this);
}

const std::vector<Vertex> getFace(glm::ivec3 pos, enum BlockSide side) {
  std::vector<Vertex> vertices;
  std::vector<glm::vec3> positions;
  switch (side) {
    case BlockSide::Front:
      positions.insert(positions.begin(), cube_front.begin(), cube_front.end());
      break;
    case BlockSide::Back:
      positions.insert(positions.begin(), cube_back.begin(), cube_back.end());
      break;
    case BlockSide::Left:
      positions.insert(positions.begin(), cube_left.begin(), cube_left.end());
      break;
    case BlockSide::Right:
      positions.insert(positions.begin(), cube_right.begin(), cube_right.end());
      break;
    case BlockSide::Bottom:
      positions.insert(positions.begin(), cube_bottom.begin(),
                       cube_bottom.end());
      break;
    case BlockSide::Up:
      positions.insert(positions.begin(), cube_up.begin(), cube_up.end());
      break;
    default:
      break;
  }
  for (const auto& vertex_position : positions) {
    Vertex v;
    v.position = vertex_position + glm::vec3(pos);

    v.data = 0;
    v.data |= (static_cast<unsigned int>(2) & 0xff) << 0;
    // v.data |= (static_cast<unsigned int>(3) << 8);
    // v.data |= (static_cast<unsigned int>(5) << 16);
    // v.data |= (static_cast<unsigned int>(7) << 24);
    vertices.push_back(v);
  }
  return (vertices);
}

void Chunk::mesh() {
  size_t total_vertices = 0;
  enum BlockSide sides[4] = {BlockSide::Left, BlockSide::Right,
                             BlockSide::Bottom, BlockSide::Up};
  for (int model_id = 0; model_id < CHUNK_HEIGHT / MODEL_HEIGHT; model_id++) {
    if (_dirty[model_id] == false) continue;
    std::vector<Vertex> vertices;
    for (int y = model_id * MODEL_HEIGHT; y < ((model_id + 1) * MODEL_HEIGHT);
         y++) {
      for (int x = 0; x < CHUNK_SIZE; x++) {
        Block current_block = {};
        for (int z = 0; z < CHUNK_SIZE; z++) {
          Block front_block = get_block({x, y, z});
          if (front_block != current_block) {
            current_block = front_block;
            auto quad = getFace({x, y, z}, BlockSide::Front);
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (z == CHUNK_SIZE - 1 && current_block.material != Material::Air) {
            auto quad = getFace({x, y, z}, BlockSide::Back);
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (x == 0 && current_block.material != Material::Air) {
            auto quad = getFace({x, y, z}, BlockSide::Right);
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (x == CHUNK_SIZE - 1 && current_block.material != Material::Air) {
            auto quad = getFace({x, y, z}, BlockSide::Left);
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (y == 0 && current_block.material != Material::Air) {
            auto quad = getFace({x, y, z}, BlockSide::Bottom);
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (y == CHUNK_SIZE - 1 && current_block.material != Material::Air) {
            auto quad = getFace({x, y, z}, BlockSide::Up);
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (current_block.material == Material::Air) {
            glm::ivec3 positions[4] = {
                glm::ivec3(x - 1, y, z),
                glm::ivec3(x + 1, y, z),
                glm::ivec3(x, y + 1, z),
                glm::ivec3(x, y - 1, z),
            };
            for (int f = 0; f < 4; f++) {
              Block b = get_block(positions[f]);
              if (b.material != Material::Air) {
                auto quad = getFace(positions[f], sides[f]);
                vertices.insert(vertices.end(), quad.begin(), quad.end());
              }
            }
          }
        }
      }
    }
    _dirty[model_id] = false;
    total_vertices += vertices.size();
    if (this->_renderAttrib.vaos.size() <= model_id) {
      this->_renderAttrib.vaos.push_back(new VAO(vertices));
    } else {
      this->_renderAttrib.vaos[model_id]->update(vertices);
    }
  }
  std::cout << "Mesher: " << total_vertices << " vertices" << std::endl;
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

ChunkManager::ChunkManager(void) : _renderDistance(0) {
  glm::ivec2 pos(0);
  for (int x = -this->_renderDistance; x <= this->_renderDistance; x++) {
    for (int z = -this->_renderDistance; z <= this->_renderDistance; z++) {
      glm::ivec2 chunk_pos(pos.x + x * CHUNK_SIZE, pos.y + z * CHUNK_SIZE);
      auto chunk = _chunks.find(chunk_pos);
      if (chunk == _chunks.end()) {
        _chunks.emplace(chunk_pos, Chunk({chunk_pos.x, 0, chunk_pos.y}));
        auto newchunk = _chunks.find(chunk_pos);
        newchunk->second.mesh();
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
  glm::ivec2 pos(0);
  for (int x = -this->_renderDistance; x <= this->_renderDistance; x++) {
    for (int z = -this->_renderDistance; z <= this->_renderDistance; z++) {
      glm::ivec2 chunk_pos(pos.x + x * CHUNK_SIZE, pos.y + z * CHUNK_SIZE);
      auto chunk = _chunks.find(chunk_pos);
      if (chunk != _chunks.end()) {
        //
      }
    }
  }
}

void ChunkManager::setRenderAttributes(Renderer& renderer) {
  auto chunk_it = _chunks.begin();
  while (chunk_it != _chunks.end()) {
    renderer.addRenderAttrib(chunk_it->second.getRenderAttrib());
    chunk_it++;
  }
}

void ChunkManager::setRenderDistance(unsigned char rd) {
  this->_renderDistance = rd;
}
