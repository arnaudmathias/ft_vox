#include "chunk.hpp"

const std::vector<glm::vec3> cube_front = {
    {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};

const std::vector<glm::vec3> cube_back = {
    {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}};

const std::vector<glm::vec3> cube_left = {
    {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f},
    {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}};

const std::vector<glm::vec3> cube_right = {
    {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};

const std::vector<glm::vec3> cube_bottom = {
    {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};

const std::vector<glm::vec3> cube_up = {{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f},
                                        {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f},
                                        {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}};

const Texture_lookup textures[4] = {{-1, -1, -1, -1, -1, -1},
                                    {1, 1, 1, 1, 1, 1},
                                    {3, 3, 3, 3, 2, 0},
                                    {18, 18, 18, 18, 18, 18}};

Chunk::Chunk() : Chunk(glm::ivec3(0)) {}

Chunk::Chunk(glm::ivec3 pos) : _pos(pos) {
  _renderAttrib.model = glm::translate(_pos);
  for (int x = 0; x < 16; x++) {
    for (int z = 0; z < 16; z++) {
      float h =
          fbm({glm::vec3(this->_pos.x + x, this->_pos.z + z, 0)}) * 0.5 + 1.0f;
      int height = round(h * 64.0);
      for (int y = 0; y < height; y++) {
        Block block;
        if (y == height - 1) {
          if (y < 67) {
            block.material = Material::Dirt;
          } else {
            block.material = Material::Stone;
          }
        } else {
          block.material = Material::Dirt;
        }
        set_block(block, glm::ivec3(x, y, z));
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

const std::vector<Vertex> getFace(const Block& block, glm::ivec3 pos,
                                  enum BlockSide side) {
  std::vector<Vertex> vertices;
  std::vector<glm::vec3> positions;

  int texture_id =
      textures[static_cast<int>(block.material)].side[static_cast<int>(side)];
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
    v.position = glm::vec4(vertex_position + glm::vec3(pos), 0.0f);
    v.position.w = static_cast<float>(side);
    v.texture_id = static_cast<float>(texture_id);
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
            Block b = front_block.material != Material::Air ? front_block
                                                            : current_block;
            auto quad = getFace(b, {x, y, z}, BlockSide::Front);
            vertices.insert(vertices.end(), quad.begin(), quad.end());
            current_block = front_block;
          }
          if (z == CHUNK_SIZE - 1 && current_block.material != Material::Air) {
            auto quad = getFace(current_block, {x, y, z}, BlockSide::Back);
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (x == 0 && current_block.material != Material::Air) {
            auto quad = getFace(current_block, {x, y, z}, BlockSide::Right);
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (x == CHUNK_SIZE - 1 && current_block.material != Material::Air) {
            auto quad = getFace(current_block, {x, y, z}, BlockSide::Left);
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (y == 0 && current_block.material != Material::Air) {
            auto quad = getFace(current_block, {x, y, z}, BlockSide::Bottom);
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (y == CHUNK_HEIGHT - 1 &&
              current_block.material != Material::Air) {
            auto quad = getFace(current_block, {x, y, z}, BlockSide::Up);
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (y == ((model_id + 1) * MODEL_HEIGHT) - 1 &&
              current_block.material != Material::Air) {
            auto quad = getFace(current_block, {x, y, z}, BlockSide::Up);
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (current_block.material == Material::Air) {
            glm::ivec3 positions[4] = {
                glm::ivec3(x - 1, y, z), glm::ivec3(x + 1, y, z),
                glm::ivec3(x, y + 1, z), glm::ivec3(x, y - 1, z)};
            for (int f = 0; f < 4; f++) {
              Block b = get_block(positions[f]);
              if (b.material != Material::Air) {
                auto quad = getFace(b, positions[f], sides[f]);
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
  // std::cout << "Mesher: " << total_vertices << " vertices" << std::endl;
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

void ChunkManager::update(glm::vec3 player_pos) {
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
  auto chunk_it = _chunks.begin();
  while (chunk_it != _chunks.end()) {
    glm::ivec3 c_pos = chunk_it->second.get_pos();
    float dist = glm::distance(glm::vec2(c_pos.x, c_pos.z), glm::vec2(pos));
    if (round(dist) / CHUNK_SIZE <
        static_cast<float>(this->_renderDistance + 1)) {
      renderer.addRenderAttrib(chunk_it->second.getRenderAttrib());
    }
    chunk_it++;
  }
}

void ChunkManager::setRenderDistance(unsigned char rd) {
  this->_renderDistance = rd;
}
