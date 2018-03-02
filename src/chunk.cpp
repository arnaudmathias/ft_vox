#include "chunk.hpp"

Chunk::Chunk() : Chunk(glm::ivec3(0)) {}

Chunk::Chunk(glm::ivec3 pos)
    : aabb_center(0.0f), aabb_halfsize(0.0f), _pos(pos) {
  _renderAttrib.model = glm::translate(_pos);
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

void Chunk::generate() {
  generator::generate_chunk(this->data, glm::vec3(_pos));
  for (int i = 0; i < MODEL_PER_CHUNK; i++) {
    this->_dirty[i] = true;
  }
}

void Chunk::mesh() {
  // mesher::greedy(data, _dirty, _renderAttrib);
  mesher::culling(data, _dirty, _renderAttrib);
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

ChunkManager::ChunkManager(void) : ChunkManager(42) {}

ChunkManager::ChunkManager(uint32_t seed) : _renderDistance(10), _seed(seed) {
  generator::init(10000, _seed);
  if (io::exists("world") == false) {
    io::makedir("world");
  }
  if (io::exists("world/" + std::to_string(_seed)) == false) {
    io::makedir("world/" + std::to_string(_seed));
  }
  loadRegion(glm::ivec2(0, 0));
}

ChunkManager::ChunkManager(ChunkManager const& src) { *this = src; }

ChunkManager::~ChunkManager(void) {
  /*
  auto chunk_it = _chunks.begin();
  while (chunk_it != _chunks.end()) {
    auto old = chunk_it;
    chunk_it++;
    glm::ivec3 c_pos = old->second.get_pos();
    io::writeRegionFile(getRegionFilename(old->first), old->first,
			old->second.data);
    _chunks.erase(old);
  }*/
}

ChunkManager& ChunkManager::operator=(ChunkManager const& rhs) {
  if (this != &rhs) {
    this->_renderDistance = rhs._renderDistance;
  }
  return (*this);
}

void ChunkManager::update(const glm::vec3& player_pos) {
  if (to_generate.size() > 0) {
    auto nearest = to_generate.begin();
    float min_dist = 99999.0f;
    auto chunk_it = to_generate.begin();
    glm::ivec2 current_pos = glm::ivec2(player_pos.x, player_pos.z);
    while (chunk_it != to_generate.end()) {
      float dist = glm::distance(glm::vec2(*chunk_it), glm::vec2(current_pos));
      if (dist < min_dist) {
	nearest = chunk_it;
      }
      chunk_it++;
    }
    // glm::ivec2 chunk_position = to_generate.front();
    auto newchunk = _chunks.find(*nearest);
    if (newchunk != _chunks.end()) {
      newchunk->second.generate();
      to_mesh.push_back(newchunk->first);
    }
    to_generate.erase(chunk_it);
  }
  if (to_mesh.size() > 0) {
    glm::ivec2 chunk_position = to_mesh.front();
    auto newchunk = _chunks.find(chunk_position);
    if (newchunk != _chunks.end()) {
      newchunk->second.mesh();
    }
    to_mesh.pop_front();
  }
  glm::ivec2 player_chunk_pos =
      glm::ivec2((static_cast<int>(round(player_pos.x)) -
		  (static_cast<int>(round(player_pos.x)) % CHUNK_SIZE)),
		 (static_cast<int>(round(player_pos.z)) -
		  (static_cast<int>(round(player_pos.z)) % CHUNK_SIZE)));
  for (int x = -this->_renderDistance; x <= this->_renderDistance; x++) {
    for (int z = -this->_renderDistance; z <= this->_renderDistance; z++) {
      glm::ivec2 chunk_pos(player_chunk_pos.x + (x * CHUNK_SIZE),
			   player_chunk_pos.y + (z * CHUNK_SIZE));
      glm::ivec2 region_pos((chunk_pos.x >> 8) * (REGION_SIZE * CHUNK_SIZE),
			    (chunk_pos.y >> 8) * (REGION_SIZE * CHUNK_SIZE));
      addRegionToQueue(region_pos);
    }
  }
  /*unloadChunks(player_chunk_pos);
  loadChunks(); */
}

void ChunkManager::addRegionToQueue(glm::ivec2 region_pos) {
  auto chunk = _chunks.find(region_pos);
  if (chunk == _chunks.end()) {
    loadRegion(region_pos);
  }
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

std::string ChunkManager::getRegionFilename(glm::ivec2 pos) {
  std::string filename = "world/" + std::to_string(_seed) + "/r." +
			 std::to_string(pos.x / REGION_SIZE) + "." +
			 std::to_string(pos.y / REGION_SIZE) + ".vox";
  return (filename);
}

void ChunkManager::loadChunks() {
  if (to_load.size() > 0) {
    int chunksPerFrame = 1;
    for (int i = 0; i < chunksPerFrame; i++) {
      if (to_load.size() == 0) break;
      glm::ivec2 chunk_position = to_load.front();
      // std::cout << chunk_position.x << "|" << chunk_position.y << std::endl;
      // std::cout << getRegionFilename(chunk_position) << std::endl;
      _chunks.emplace(chunk_position,
		      Chunk({chunk_position.x, 0, chunk_position.y}));
      auto newchunk = _chunks.find(chunk_position);
      /*
      newchunk->second.generate();
      io::writeRegionFile(getRegionFilename(newchunk->first), newchunk->first,
			  newchunk->second.data);*/
      if (io::readRegionFile(getRegionFilename(newchunk->first),
			     newchunk->first, newchunk->second.data) == false) {
	// std::cout << "generate" << std::endl;
	newchunk->second.generate();
      } else {
	// std::cout << "read" << std::endl;
      }
      newchunk->second.mesh();
      to_load.pop_front();
    }
  }
}

void ChunkManager::loadRegion(glm::ivec2 region_pos) {
  unsigned char chunk_rle[(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT) * 2] = {0};
  unsigned char lookup[REGION_LOOKUPTABLE_SIZE] = {0};

  std::string filename = getRegionFilename(region_pos);
  if (io::exists(filename) == false) {
    io::initRegionFile(filename);
  }
  FILE* region = fopen(filename.c_str(), "r+b");
  if (region != NULL) {
    fread(lookup, REGION_LOOKUPTABLE_SIZE, 1, region);

    for (int y = 0; y < REGION_SIZE; y++) {
      for (int x = 0; x < REGION_SIZE; x++) {
	int lookup_offset = 4 * (x + y * REGION_SIZE);
	unsigned int offset =
	    ((unsigned int)(lookup[lookup_offset + 0]) << 16) |
	    ((unsigned int)(lookup[lookup_offset + 1]) << 8) |
	    ((unsigned int)(lookup[lookup_offset + 2]));
	unsigned int sector_count = (lookup[lookup_offset + 3] & 0xff);

	glm::ivec2 chunk_position = glm::ivec2(region_pos.x + (x * CHUNK_SIZE),
					       region_pos.y + (y * CHUNK_SIZE));
	_chunks.emplace(chunk_position,
			Chunk({chunk_position.x, 0, chunk_position.y}));

	auto chunk = _chunks.find(chunk_position);
	if (sector_count != 0) {
	  fseek(region, offset + REGION_LOOKUPTABLE_SIZE, SEEK_SET);
	  fread(chunk_rle, sector_count * SECTOR_OFFSET, 1, region);
	  io::decodeRLE(chunk_rle, sector_count * SECTOR_OFFSET,
			chunk->second.data);
	  this->to_mesh.push_back(chunk->first);
	} else {
	  this->to_generate.push_back(chunk->first);
	  // chunk->second.generate();
	}
      }
    }
    fclose(region);
  }
}

void ChunkManager::unloadRegion(glm::ivec2 region_pos) {
  unsigned char chunk_rle[(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT) * 2] = {0};
  unsigned char lookup[REGION_LOOKUPTABLE_SIZE] = {0};

  std::string filename = getRegionFilename(region_pos);
  if (io::exists(filename) == false) {
    io::initRegionFile(filename);
  }
  FILE* region = fopen(filename.c_str(), "r+b");
  if (region != NULL) {
    fread(lookup, REGION_LOOKUPTABLE_SIZE, 1, region);

    for (int y = 0; y < REGION_SIZE; y++) {
      for (int x = 0; x < REGION_SIZE; x++) {
	int lookup_offset = 4 * (x + y * REGION_SIZE);
	unsigned int offset =
	    ((unsigned int)(lookup[lookup_offset + 0]) << 16) |
	    ((unsigned int)(lookup[lookup_offset + 1]) << 8) |
	    ((unsigned int)(lookup[lookup_offset + 2]));
	unsigned int sector_count = (lookup[lookup_offset + 3] & 0xff);

	glm::ivec2 chunk_position = glm::ivec2(region_pos.x + (x * CHUNK_SIZE),
					       region_pos.y + (y * CHUNK_SIZE));

	auto chunk = _chunks.find(chunk_position);
	size_t len_rle = io::encodeRLE(chunk->second.data, chunk_rle);
	lookup[lookup_offset + 3] = (len_rle / SECTOR_OFFSET) + 1;
	fseek(region, offset + REGION_LOOKUPTABLE_SIZE, SEEK_SET);
	fwrite(chunk_rle, len_rle, 1, region);
	_chunks.erase(chunk);
      }
    }
    fseek(region, 0, SEEK_SET);
    fwrite(chunk_rle, REGION_LOOKUPTABLE_SIZE, 1, region);
    fclose(region);
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
      io::writeRegionFile(getRegionFilename(old->first), old->first,
			  old->second.data);
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
