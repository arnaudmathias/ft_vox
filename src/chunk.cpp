#include "chunk.hpp"
#include <stb_image.h>

float mapp(float unscaledNum, float minAllowed, float maxAllowed, float min,
	   float max) {
  return (maxAllowed - minAllowed) * (unscaledNum - min) / (max - min) +
	 minAllowed;
}
enum Color {
  BLACK = 0,
  BLUE = 1,
  GREEN = 2,
  CYAN = 3,
  RED = 4,
  VIOLET = 5,
  KAKI = 6,
  WHITE = 7
};

Chunk::Chunk() : Chunk(glm::ivec3(0)) {}

Chunk::Chunk(glm::ivec3 pos)
    : aabb_center(0.0f), aabb_halfsize(0.0f), _pos(pos), generated(false) {
  _renderAttrib.model = glm::translate(_pos);
  for (int i = 0; i < MODEL_PER_CHUNK; i++) {
    this->dirty[i] = true;
  }
}

Chunk::Chunk(Chunk const& src) { *this = src; }

Chunk::~Chunk(void) {
  for (auto& vao : _renderAttrib.vaos) {
    delete vao;
  }
}

Material get_material_color(Color c) {
  Block block(Material::Grey);
  return static_cast<Material>((int)block.material + static_cast<int>(c));
}

Chunk& Chunk::operator=(Chunk const& rhs) {
  if (this != &rhs) {
    this->_pos = rhs._pos;
    std::memcpy(this->data, rhs.data, sizeof(this->data));
    this->aabb_center = rhs.aabb_center;
    this->aabb_halfsize = rhs.aabb_halfsize;
    this->_renderAttrib.vaos = rhs._renderAttrib.vaos;
    this->_renderAttrib.model = rhs._renderAttrib.model;
    std::memcpy(this->dirty, rhs.dirty, sizeof(this->dirty));
    this->_renderAttrib = rhs._renderAttrib;
    this->_pos = rhs._pos;
    this->generated = rhs.generated;
  }
  return (*this);
}

void Chunk::generate() {
  generated = true;
  generator::generate_chunk(this->data, this->biome_data, glm::vec3(_pos));
  for (int i = 0; i < MODEL_PER_CHUNK; i++) {
    this->dirty[i] = true;
  }
}

void Chunk::mesh(enum MeshingType meshing_type) {
  if (is_dirty()) {
    if (_renderAttrib.vaos.size() != CHUNK_HEIGHT / MODEL_HEIGHT) {
      _renderAttrib.vaos.resize(CHUNK_HEIGHT / MODEL_HEIGHT);
    }
    switch (meshing_type) {
      case MeshingType::Culling:
	mesher::culling(this, _renderAttrib);
	break;
      case MeshingType::Greedy:
	mesher::greedy(this, _renderAttrib);
	break;
      default:
	break;
    }
    mesher::get_aabb(data, aabb_center, aabb_halfsize, _pos);
  }
};

const RenderAttrib& Chunk::getRenderAttrib() { return (this->_renderAttrib); }

enum BlockSide get_face(std::string last_step, glm::ivec3 sign) {
  if (last_step == "x") {
    if (sign.x == -1) {
      return BlockSide::Left;
    }
    return BlockSide::Right;
  } else if (last_step == "y") {
    if (sign.y == 1) {
      return BlockSide::Bottom;
    }
    return BlockSide::Up;
  } else {
    if (sign.z == 1) {
      return BlockSide::Back;
    }
    return BlockSide::Front;
  }
}

inline Block Chunk::get_block(glm::ivec3 index) {
  if (index.x < 0 || index.x >= CHUNK_SIZE || index.y < 0 || index.y >= 256 ||
      index.z < 0 || index.z >= CHUNK_SIZE) {
    Block block = {};
    return (block);
  }
  return (this->data[index.y * CHUNK_SIZE * CHUNK_SIZE + index.x * CHUNK_SIZE +
		     index.z]);
}

glm::mat4 Chunk::get_model_matrix() { return (this->_renderAttrib.model); }

inline Biome Chunk::get_biome(glm::ivec3 index) {
  if (index.x < 0 || index.x >= CHUNK_SIZE || index.z < 0 ||
      index.z >= CHUNK_SIZE) {
    Biome biome = {};
    return (biome);
  }
  return (this->biome_data[index.x * CHUNK_SIZE + index.z]);
}

inline void Chunk::set_block(Block block, glm::ivec3 index) {
  if (index.x < 0 || index.x >= CHUNK_SIZE || index.y < 0 || index.y >= 256 ||
      index.z < 0 || index.z >= CHUNK_SIZE) {
    return;
  }
  this->data[index.y * CHUNK_SIZE * CHUNK_SIZE + index.x * CHUNK_SIZE +
	     index.z] = block;
  this->dirty[index.y / MODEL_HEIGHT] = true;
}

glm::ivec3 Chunk::get_pos() { return (_pos); }

bool Chunk::is_dirty() {
  for (int i = 0; i < MODEL_PER_CHUNK; i++) {
    if (dirty[i] == true) {
      return (true);
    }
  }
  return (false);
}

void Chunk::forceFullRemesh() {
  for (int i = 0; i < MODEL_PER_CHUNK; i++) {
    this->dirty[i] = true;
  }
}

void ChunkManager::add_block(glm::ivec3 index) {
  set_block(_current_block, index);
}

void ChunkManager::point_exploding(glm::ivec3 index, float intensity) {
  float random;
  srand(time(nullptr));
  for (int x = index.x - intensity; x < index.x + intensity; x++) {
    for (int y = index.y - intensity; y < index.y + intensity; y++) {
      for (int z = index.z - intensity; z < index.z + intensity; z++) {
	if (glm::distance(glm::vec3(x, y, z), glm::vec3(index)) < intensity)
	  if (rand() / static_cast<float>(RAND_MAX) * 0.3 + 0.7 >
	      glm::distance(glm::vec3(x, y, z), glm::vec3(index)) / intensity) {
	    set_block(Block(Material::Air), glm::ivec3(x, y, z));
	  }
      }
    }
  }
}

void Chunk::setDirty(int model_id) { dirty[model_id] = true; }

ChunkManager::ChunkManager(void) : ChunkManager(42) {}

ChunkManager::ChunkManager(uint32_t seed)
    : _renderDistance(10), _seed(seed), _meshing_type(MeshingType::Greedy) {
  generator::init(10000, _seed);
  if (io::exists("world") == false) {
    io::makedir("world");
  }
  if (io::exists("world/" + std::to_string(_seed)) == false) {
    io::makedir("world/" + std::to_string(_seed));
  }
}

ChunkManager::ChunkManager(ChunkManager const& src) { *this = src; }

ChunkManager::~ChunkManager(void) {
  std::unordered_set<glm::ivec2, ivec2Comparator> regions;
  auto chunk_it = _chunks.begin();
  while (chunk_it != _chunks.end()) {
    glm::ivec2 chunk_pos(chunk_it->first);
    glm::ivec2 region_pos((chunk_pos.x >> 8) * (REGION_SIZE * CHUNK_SIZE),
			  (chunk_pos.y >> 8) * (REGION_SIZE * CHUNK_SIZE));
    regions.insert(region_pos);
    chunk_it++;
  }
  for (const auto& region_pos : regions) {
    unloadRegion(region_pos);
  }
}

ChunkManager& ChunkManager::operator=(ChunkManager const& rhs) {
  if (this != &rhs) {
    this->_renderDistance = rhs._renderDistance;
    this->_meshing_type = rhs._meshing_type;
  }
  return (*this);
}

inline unsigned int getNearestIdx(glm::vec2 position,
				  const std::deque<glm::ivec2>& positions) {
  unsigned int nearest = 0;
  float min_dist = 99999.0f;
  for (unsigned int i = 0; i < positions.size(); i++) {
    float dist = glm::distance(position, glm::vec2(positions[i]));
    if (dist < min_dist) {
      nearest = i;
      min_dist = dist;
    }
  }
  return (nearest);
}

void ChunkManager::update(const glm::vec3& player_pos) {
  if (to_update.size() > 0) {
    // unsigned int nearest_idx =
    //   getNearestIdx(glm::vec2(player_pos.x, player_pos.z), to_update);
    auto nearest_chunk_it = _chunks.find(to_update.front());
    // auto nearest_chunk_it = _chunks.find(to_update[nearest_idx]);
    if (nearest_chunk_it != _chunks.end()) {
      nearest_chunk_it->second.mesh(this->_meshing_type);
      to_mesh.push_back(nearest_chunk_it->first);
    }
    to_update.pop_front();
    // to_update.erase(to_update.begin() + nearest_idx);
  }
  // Find nearest chunk and gen it
  if (to_generate.size() > 0) {
    unsigned int nearest_idx =
	getNearestIdx(glm::vec2(player_pos.x, player_pos.z), to_generate);
    auto nearest_chunk_it = _chunks.find(to_generate[nearest_idx]);
    if (nearest_chunk_it != _chunks.end()) {
      nearest_chunk_it->second.generate();
      to_mesh.push_back(nearest_chunk_it->first);
    }
    to_generate.erase(to_generate.begin() + nearest_idx);
  }
  // Find nearest chunk and mesh it
  if (to_mesh.size() > 0) {
    unsigned int nearest_idx =
	getNearestIdx(glm::vec2(player_pos.x, player_pos.z), to_mesh);
    auto nearest_chunk_it = _chunks.find(to_mesh[nearest_idx]);
    if (nearest_chunk_it != _chunks.end()) {
      nearest_chunk_it->second.mesh(this->_meshing_type);
    }
    to_mesh.erase(to_mesh.begin() + nearest_idx);
  }
  // Add regions within renderDistance
  glm::ivec2 player_chunk_pos =
      glm::ivec2((static_cast<int>(player_pos.x) >> 4) * CHUNK_SIZE,
		 (static_cast<int>(player_pos.z) >> 4) * CHUNK_SIZE);
  for (int x = -this->_renderDistance; x <= this->_renderDistance; x++) {
    for (int z = -this->_renderDistance; z <= this->_renderDistance; z++) {
      glm::ivec2 chunk_pos(player_chunk_pos.x + (x * CHUNK_SIZE),
			   player_chunk_pos.y + (z * CHUNK_SIZE));
      glm::ivec2 region_pos((chunk_pos.x >> 8) * (REGION_SIZE * CHUNK_SIZE),
			    (chunk_pos.y >> 8) * (REGION_SIZE * CHUNK_SIZE));
      addRegionToQueue(region_pos);
    }
  }
  // Unload regions
  unloadRegions(player_chunk_pos);
  if (to_unload.size() > 0) {
    unloadRegion(to_unload.front());
    to_unload.pop_front();
  }
}

Color get_best_color(glm::vec3 earth_color) {
  glm::vec3 color[8] = {
      {0.0, 0.0, 0.0},  {0.0, 0.0, 0x99},  {0.0, 0x99, 0.0},  {0.0, 0x99, 0x99},
      {0x99, 0.0, 0.0}, {0x99, 0.0, 0x99}, {0x99, 0x99, 0.0}, {1.0, 0.0, 0.0},
  };

  int best = 1;
  float best_dist = 999999;
  // std::cout << earth_color.x << "|" << earth_color.y << "|"<< earth_color.z
  // << std::endl;
  float hEarth = glm::hsvColor(earth_color).x;
  for (int i = 0; i < 8; i++) {
    if (glm::abs(hEarth - glm::hsvColor(color[i]).x) < best_dist) {
      best_dist = glm::abs(hEarth - glm::hsvColor(color[i]).x);
      best = i;
    }
  }
  return static_cast<Color>(best);
}

void ChunkManager::Draw_earth() {
  int size = 100;
  int w, h;
  int texChannels = 0;
  stbi_uc* pixels =
      stbi_load("./textures/earth.jpg", &w, &h, &texChannels, STBI_rgb_alpha);
  for (int x = -size; x < size; x++) {
    for (int y = -size; y < size; y++) {
      for (int z = -size; z < size; z++) {
	glm::vec3 normal(x + 0.5, y + 0.5, z + 0.5);
	normal = glm::normalize(normal);
	float uu = atan2(normal.z, normal.x);
	float vv = asin(normal.y);

	// int u = mapp(uu, -M_PI, M_PI, 0, w);
	int u = mapp(uu, 0, w, -M_PI, M_PI);
	// int v = mapp(vv, -(M_PI / 2.0), M_PI / 2.0, 0, h);
	int v = mapp(vv, 0, h, -(M_PI / 2.0), M_PI / 2.0);

	glm::vec3 color(pixels[(v * w + u) * 4], pixels[(v * w + u + 1) * 4],
			pixels[(v * w + u + 2) * 4]);
	int idmax = 0;
	float mindist = 9999;
	int arr[5] = {130, 162, 194, 178, 178};
	if (glm::distance(glm::vec3(x, y, z), glm::vec3(0.0)) < size / 2 &&
	    glm::distance(glm::vec3(x, y, z), glm::vec3(0.0)) > size / 2 - 1) {
	  Block block(Material::Grey);
	  Color c = get_best_color(color);
	  block.material = get_material_color(c);
	  set_block(block, glm::ivec3(x, y, z));

	} else {
	  set_block(Block(Material::Air), glm::ivec3(x, y, z));
	}
      }
    }
  }
}

void ChunkManager::addRegionToQueue(glm::ivec2 region_pos) {
  auto chunk = _chunks.find(region_pos);
  if (chunk == _chunks.end()) {
    // TODO : Defer this (1 max per frame)
    loadRegion(region_pos);
  }
}

std::string ChunkManager::getRegionFilename(glm::ivec2 pos) {
  std::string filename = "world/" + std::to_string(_seed) + "/r." +
			 std::to_string(pos.x / REGION_SIZE) + "." +
			 std::to_string(pos.y / REGION_SIZE) + ".vox";
  return (filename);
}

void ChunkManager::loadRegion(glm::ivec2 region_pos) {
  unsigned char chunk_rle[(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT) * 2] = {0};
  unsigned char lookup[REGION_LOOKUPTABLE_SIZE] = {0};

  std::string filename = getRegionFilename(region_pos);
  if (io::exists(filename) == false) {
    io::initRegionFile(filename);
  }
  unsigned int filesize = io::get_filesize(filename);
  FILE* region = fopen(filename.c_str(), "rb+");
  if (region != NULL) {
    if (filesize < REGION_LOOKUPTABLE_SIZE) {
      fclose(region);
      return;
    }
    fread(lookup, REGION_LOOKUPTABLE_SIZE, 1, region);

    int file_offset = REGION_LOOKUPTABLE_SIZE;
    for (int y = 0; y < REGION_SIZE; y++) {
      for (int x = 0; x < REGION_SIZE; x++) {
	int lookup_offset = 3 * (x + y * REGION_SIZE);
	unsigned int content_size =
	    ((unsigned int)(lookup[lookup_offset + 0]) << 16) |
	    ((unsigned int)(lookup[lookup_offset + 1]) << 8) |
	    ((unsigned int)(lookup[lookup_offset + 2]));

	glm::ivec2 chunk_position = glm::ivec2(region_pos.x + (x * CHUNK_SIZE),
					       region_pos.y + (y * CHUNK_SIZE));
	auto emplace_res = _chunks.emplace(
	    chunk_position, Chunk({chunk_position.x, 0, chunk_position.y}));
	auto chunk_it = emplace_res.first;
	if (content_size != 0) {
	  // Chunk already generated and saved on disk, just decode and mesh
	  // it back
	  if (filesize < file_offset + content_size) {
	    fclose(region);
	    return;
	  }
	  fseek(region, file_offset, SEEK_SET);
	  fread(chunk_rle, content_size, 1, region);
	  io::decodeRLE(chunk_rle, content_size, chunk_it->second.data,
			(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT));
	  chunk_it->second.generated = true;
	  this->to_mesh.push_back(chunk_it->first);
	} else {
	  this->to_generate.push_back(chunk_it->first);
	}
	file_offset += content_size;
      }
    }
    fclose(region);
  }
}

void ChunkManager::eraseUnloadedChunk(glm::ivec2 pos) {
  // Remove chunk from queues
  for (auto it = to_mesh.begin(); it != to_mesh.end(); it++) {
    if (*it == pos) {
      to_mesh.erase(it);
      break;
    }
  }
  for (auto it = to_generate.begin(); it != to_generate.end(); it++) {
    if (*it == pos) {
      to_generate.erase(it);
      break;
    }
  }
}

void ChunkManager::unloadRegion(glm::ivec2 region_pos) {
  unsigned char chunk_rle[(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT) * 2] = {0};
  unsigned char lookup[REGION_LOOKUPTABLE_SIZE] = {0};

  std::string filename = getRegionFilename(region_pos);
  if (io::exists(filename) == false) {
    io::initRegionFile(filename);
  }
  FILE* region = fopen(filename.c_str(), "rb+");
  if (region != NULL) {
    fread(lookup, REGION_LOOKUPTABLE_SIZE, 1, region);

    int file_offset = REGION_LOOKUPTABLE_SIZE;
    for (int y = 0; y < REGION_SIZE; y++) {
      for (int x = 0; x < REGION_SIZE; x++) {
	int lookup_offset = 3 * (x + y * REGION_SIZE);
	unsigned int content_size = 0;

	glm::ivec2 chunk_position = glm::ivec2(region_pos.x + (x * CHUNK_SIZE),
					       region_pos.y + (y * CHUNK_SIZE));
	auto chunk_it = _chunks.find(chunk_position);
	if (chunk_it != _chunks.end()) {
	  if (chunk_it->second.generated) {
	    std::memset(chunk_rle, 0,
			(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT) * 2);
	    unsigned int len_rle = static_cast<unsigned int>(
		io::encodeRLE(chunk_it->second.data, chunk_rle));
	    lookup[lookup_offset + 0] = (len_rle & 0xff0000) >> 16;
	    lookup[lookup_offset + 1] = (len_rle & 0xff00) >> 8;
	    lookup[lookup_offset + 2] = (len_rle & 0xff);
	    content_size = len_rle;
	    fseek(region, file_offset, SEEK_SET);
	    fwrite(chunk_rle, len_rle, 1, region);
	  }
	  eraseUnloadedChunk(chunk_it->first);
	  _chunks.erase(chunk_it);
	}
	file_offset += content_size;
      }
    }
    fseek(region, 0, SEEK_SET);
    fwrite(lookup, REGION_LOOKUPTABLE_SIZE, 1, region);
    fclose(region);
  }
}

void ChunkManager::unloadRegions(glm::ivec2 current_chunk_pos) {
  std::unordered_set<glm::ivec2, ivec2Comparator> regions;
  auto chunk_it = _chunks.begin();
  while (chunk_it != _chunks.end()) {
    glm::ivec2 chunk_pos(chunk_it->first);
    glm::ivec2 region_pos((chunk_pos.x >> 8) * (REGION_SIZE * CHUNK_SIZE),
			  (chunk_pos.y >> 8) * (REGION_SIZE * CHUNK_SIZE));
    regions.insert(region_pos);
    chunk_it++;
  }
  if (regions.size() > 0) {
    for (auto region : regions) {
      glm::ivec2 region_center = region + ((REGION_SIZE * CHUNK_SIZE) / 2);
      float dist = fabs(glm::distance(glm::vec2(current_chunk_pos),
				      glm::vec2(region_center)));
      if (round(dist) / CHUNK_SIZE >
	  static_cast<float>(this->_renderDistance + (REGION_SIZE + 1))) {
	to_unload.push_back(region);
      }
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

  _debug_chunks_rendered = 0;
  auto chunk_it = _chunks.begin();
  while (chunk_it != _chunks.end()) {
    glm::ivec3 c_pos = chunk_it->second.get_pos();
    float dist = glm::distance(glm::vec2(c_pos.x, c_pos.z), glm::vec2(pos));
    if (round(dist) / CHUNK_SIZE <
	static_cast<float>(this->_renderDistance + 1)) {
      if (frustrum_culling.cull(chunk_it->second.aabb_center,
				chunk_it->second.aabb_halfsize)) {
	renderer.addRenderAttrib(chunk_it->second.getRenderAttrib());
	_debug_chunks_rendered++;
      }
    }
    chunk_it++;
  }
}

inline Block ChunkManager::get_block(glm::ivec3 index) {
  glm::ivec2 chunk_pos =
      glm::ivec2((index.x >> 4) * CHUNK_SIZE, (index.z >> 4) * CHUNK_SIZE);
  auto chunk_it = _chunks.find(chunk_pos);
  if (chunk_it != _chunks.end()) {
    glm::ivec3 block_pos;
    block_pos.x = index.x - chunk_pos.x;
    block_pos.y = index.y;
    block_pos.z = index.z - chunk_pos.y;
    return (chunk_it->second.get_block(block_pos));
  }
  Block block = {};
  return (block);
}
inline glm::mat4 ChunkManager::get_model_matrix(glm::ivec3 index) {
  glm::ivec2 chunk_pos =
      glm::ivec2((index.x >> 4) * CHUNK_SIZE, (index.z >> 4) * CHUNK_SIZE);
  auto chunk_it = _chunks.find(chunk_pos);
  if (chunk_it != _chunks.end()) {
    glm::ivec3 block_pos;
    block_pos.x = index.x - chunk_pos.x;
    block_pos.y = index.y;
    block_pos.z = index.z - chunk_pos.y;
    return (chunk_it->second.get_model_matrix());
  }
  glm::mat4 m(1.0f);
  return (m);
}

void ChunkManager::set_block(Block block, glm::ivec3 index) {
  glm::ivec2 chunk_pos =
      glm::ivec2((index.x >> 4) * CHUNK_SIZE, (index.z >> 4) * CHUNK_SIZE);
  auto chunk_it = _chunks.find(chunk_pos);
  if (chunk_it != _chunks.end()) {
    glm::ivec3 block_pos;
    block_pos.x = index.x - chunk_pos.x;
    block_pos.y = index.y;
    block_pos.z = index.z - chunk_pos.y;
    if (_meshing_type == MeshingType::Greedy) {
      chunk_it->second.forceFullRemesh();
    }
    chunk_it->second.set_block(block, block_pos);
    bool found = false;
    for (int i = 0; i < to_update.size(); i++) {
      if (to_update[i] == chunk_it->first) {
	found = true;
	break;
      }
    }
    if (found == false) {
      to_update.push_back(chunk_it->first);
    }
  }
}

inline float intbound(float pos, float ds) {
  return (ds > 0.0f ? ceil(pos) - pos : pos - floor(pos)) / fabs(ds);
}

struct HitInfo ChunkManager::rayCast(glm::vec3 ray_dir, glm::vec3 ray_pos,
				     float max_dist) {
  // http://www.cse.chalmers.se/edu/year/2011/course/TDA361/grid.pdf
  glm::ivec3 pos = glm::floor(ray_pos);
  struct HitInfo info = {};
  bool sign;
  std::string last_step = "none";
  glm::ivec3 step;
  step.x = ray_dir.x < 0.0f ? -1 : 1;
  step.y = ray_dir.y < 0.0f ? -1 : 1;
  step.z = ray_dir.z < 0.0f ? -1 : 1;
  glm::vec3 tMax;
  glm::vec3 delta = glm::vec3(step) / ray_dir;
  tMax.x = intbound(ray_pos.x, ray_dir.x);
  tMax.y = intbound(ray_pos.y, ray_dir.y);
  tMax.z = intbound(ray_pos.z, ray_dir.z);
  Block block(Material::Air);
  info.hit = 0;
  while (1) {
    if (pos.y > 255 || pos.y < 0) {
      info.hit = false;
      break;
    }
    if (block.material != Material::Air) {
      info.hit = true;
      break;
    }
    if (tMax.x < tMax.y) {
      if (tMax.x < tMax.z) {
	if (tMax.x > max_dist) break;
	pos.x += step.x;
	info.pos = pos;
	last_step = "x";
	tMax.x += delta.x;
      } else {
	if (tMax.z > max_dist) break;
	pos.z += step.z;
	info.pos = pos;
	last_step = "z";
	tMax.z += delta.z;
      }
    } else {
      if (tMax.y < tMax.z) {
	if (tMax.y > max_dist) break;
	pos.y += step.y;
	info.pos = pos;
	tMax.y += delta.y;
	last_step = "y";
      } else {
	if (tMax.z > max_dist) break;
	last_step = "z";
	pos.z += step.z;
	info.pos = pos;
	tMax.z += delta.z;
      }
    }
    block = get_block(pos);
  }
  if (info.hit) {
    info.side = get_face(last_step, step);
    info.model = get_model_matrix(pos);
  }
  return info;
}

void ChunkManager::setRenderDistance(unsigned char rd) {
  this->_renderDistance = rd;
}

void ChunkManager::increaseRenderDistance() {
  if (this->_renderDistance + 1 <= 20) this->_renderDistance++;
}

void ChunkManager::decreaseRenderDistance() {
  if (this->_renderDistance - 1 > 0) this->_renderDistance--;
}

void ChunkManager::setMeshingType(enum MeshingType type) {
  _meshing_type = type;
  reloadMesh();
}

void ChunkManager::setBlockType(struct Block type) { _current_block = type; }

void ChunkManager::reloadMesh() {
  to_mesh.clear();
  auto chunk_it = _chunks.begin();
  while (chunk_it != _chunks.end()) {
    chunk_it->second.forceFullRemesh();
    to_mesh.push_back(chunk_it->first);
    chunk_it++;
  }
}

void ChunkManager::print_chunkmanager_info(Renderer& renderer, float fheight,
					   float fwidth) {
  renderer.renderText(
      10.0f, fheight - 75.0f, 0.35f,
      "chunks: " + std::to_string(_chunks.size()) +
	  ", rendered: " + std::to_string(_debug_chunks_rendered),
      glm::vec3(1.0f, 1.0f, 1.0f));
  renderer.renderText(10.0f, fheight - 100.0f, 0.35f,
		      "queue: mesh(" + std::to_string(to_mesh.size()) +
			  ") priority(" + std::to_string(to_update.size()) +
			  ") generate(" + std::to_string(to_generate.size()) +
			  ") unload(" + std::to_string(to_unload.size()) + ")",
		      glm::vec3(1.0f, 1.0f, 1.0f));
  renderer.renderText(10.0f, fheight - 125.0f, 0.35f,
		      "render distance: " + std::to_string(_renderDistance),
		      glm::vec3(1.0f, 1.0f, 1.0f));
}
