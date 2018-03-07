#include "meshing.hpp"

const std::vector<glm::vec3> cube = {

    {0.0f, 1.0f, 0.0f},
    {1.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},

    {0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 1.0f},

    {1.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {1.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 1.0f},

    {0.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 1.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},

    {0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 1.0f}

};

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

namespace mesher {

const std::vector<Vertex> getFace(Chunk *chunk, const Block &block,
                                  glm::ivec3 pos, enum BlockSide side,
                                  glm::vec3 scale) {
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
  for (size_t i = 0; i < positions.size(); i++) {
    positions.at(i) *= scale;
  }
  // positions = get_scaled_cube(scale);
  for (const auto &vertex_position : positions) {
    Vertex v;
    v.position = glm::vec4(vertex_position + glm::vec3(pos), 0.0f);
    v.position.w = static_cast<float>(side);
    v.attribs.x = static_cast<float>(texture_id);
    v.attribs.y =
        static_cast<float>(chunk->biome_data[pos.x * CHUNK_SIZE + pos.z]);
    vertices.push_back(v);
  }
  return (vertices);
}

std::vector<Vertex> get_scaled_cube(Chunk *chunk, Block b, glm::vec3 scale,
                                    glm::ivec3 pos) {
  std::vector<Vertex> vertices;

  auto quad = getFace(chunk, b, pos, BlockSide::Front, scale);
  vertices.insert(vertices.end(), quad.begin(), quad.end());
  quad = getFace(chunk, b, pos, BlockSide::Right, scale);
  vertices.insert(vertices.end(), quad.begin(), quad.end());
  quad = getFace(chunk, b, pos, BlockSide::Back, scale);
  vertices.insert(vertices.end(), quad.begin(), quad.end());
  quad = getFace(chunk, b, pos, BlockSide::Left, scale);
  vertices.insert(vertices.end(), quad.begin(), quad.end());
  quad = getFace(chunk, b, pos, BlockSide::Bottom, scale);
  vertices.insert(vertices.end(), quad.begin(), quad.end());
  quad = getFace(chunk, b, pos, BlockSide::Up, scale);
  vertices.insert(vertices.end(), quad.begin(), quad.end());

  return vertices;
}

glm::ivec3 get_interval(Block *data, glm::ivec3 pos, Block current_block) {
  glm::ivec3 save_pos = pos;
  glm::ivec3 size = glm::ivec3(0);

  Block front_block = get_block(data, {pos.x, pos.y, pos.z});
  while (pos.x < CHUNK_SIZE) {
    front_block = get_block(data, {pos.x, pos.y, pos.z});
    if (front_block != current_block) break;
    size.x++;
    pos.x++;
  }
  pos = save_pos;
  front_block = get_block(data, {pos.x, pos.y, pos.z});
  while (pos.y < CHUNK_HEIGHT) {
    while (size.x > 0 && pos.x - save_pos.x < size.x) {
      front_block = get_block(data, {pos.x, pos.y, pos.z});
      if (front_block != current_block) break;
      pos.x++;
    }
    if (front_block != current_block) break;
    size.y++;
    pos.x = save_pos.x;
    pos.y++;
  }
  pos = save_pos;
  front_block = get_block(data, {pos.x, pos.y, pos.z});
  while (pos.z < CHUNK_SIZE) {
    while (size.y > 0 && pos.y - save_pos.y < size.y) {
      while (size.x > 0 && pos.x - save_pos.x < size.x) {
        front_block = get_block(data, {pos.x, pos.y, pos.z});
        if (front_block != current_block) break;
        pos.x++;
      }
      if (front_block != current_block) break;
      pos.x = save_pos.x;
      pos.y++;
    }
    if (front_block != current_block) break;
    pos.z++;
    size.z++;
    pos.x = save_pos.x;
    pos.y = save_pos.y;
  }
  return size;
}

bool is_fill(std::vector<glm::ivec2> interval_dimension[3], glm::ivec3 pos) {
  for (size_t i = 0; i < interval_dimension[0].size(); ++i) {
    if (interval_dimension[0].at(i).x <= pos.x &&
        interval_dimension[0].at(i).y > pos.x &&
        interval_dimension[1].at(i).x <= pos.y &&
        interval_dimension[1].at(i).y > pos.y &&
        interval_dimension[2].at(i).x <= pos.z &&
        interval_dimension[2].at(i).y > pos.z)
      return true;
  }
  return false;
}

void greedy(Chunk *chunk, RenderAttrib &render_attrib) {
  size_t total_vertices = 0;
  enum BlockSide sides[4] = {BlockSide::Left, BlockSide::Right,
                             BlockSide::Bottom, BlockSide::Up};
  glm::ivec3 inter = glm::ivec3(0);
  std::vector<glm::ivec2> interval_dimension[3] = {{}};
  for (unsigned int model_id = 0; model_id < MODEL_PER_CHUNK; model_id++) {
    if (chunk->dirty[model_id] == false) continue;
    std::vector<Vertex> vertices;
    for (int y = model_id * MODEL_HEIGHT; y < ((model_id + 1) * MODEL_HEIGHT);
         y++) {
      for (int x = 0; x < CHUNK_SIZE; x++) {
        Block current_block = {};
        for (int z = 0; z < CHUNK_SIZE; z++) {
          Block front_block = get_block(chunk->data, {x, y, z});
          if (front_block.material != Material::Air &&
              !is_fill(interval_dimension, {x, y, z})) {
            Block b = front_block.material != Material::Air ? front_block
                                                            : current_block;
            inter = get_interval(chunk->data, glm::ivec3(x, y, z), b);
            interval_dimension[0].push_back({x, x + inter.x});
            interval_dimension[1].push_back({y, y + inter.y});
            interval_dimension[2].push_back({z, z + inter.z});
            auto cube = get_scaled_cube(chunk, b, inter, {x, y, z});
            vertices.insert(vertices.begin(), cube.begin(), cube.end());
            current_block = front_block;
          }
        }
      }
    }
    chunk->dirty[model_id] = false;
    total_vertices += vertices.size();
    if (vertices.size() > 0) {
      if (render_attrib.vaos.size() <= model_id) {
        render_attrib.vaos.push_back(new VAO(vertices));
      } else {
        render_attrib.vaos[model_id]->update(vertices);
      }
    }
  }
  // std::cout << "Mesher: " << total_vertices << " vertices" << std::endl;
}
void culling(Chunk *chunk, RenderAttrib &render_attrib) {
  size_t total_vertices = 0;
  enum BlockSide sides[4] = {BlockSide::Left, BlockSide::Right,
                             BlockSide::Bottom, BlockSide::Up};
  for (unsigned int model_id = 0; model_id < CHUNK_HEIGHT / MODEL_HEIGHT;
       model_id++) {
    if (chunk->dirty[model_id] == false) continue;
    std::vector<Vertex> vertices;
    for (int y = model_id * MODEL_HEIGHT; y < ((model_id + 1) * MODEL_HEIGHT);
         y++) {
      for (int x = 0; x < CHUNK_SIZE; x++) {
        Block current_block = {};
        for (int z = 0; z < CHUNK_SIZE; z++) {
          Block front_block = get_block(chunk->data, {x, y, z});
          if (front_block != current_block) {
            Block b = front_block.material != Material::Air ? front_block
                                                            : current_block;
            auto quad =
                getFace(chunk, b, {x, y, z}, BlockSide::Front, glm::vec3(1.0f));
            vertices.insert(vertices.end(), quad.begin(), quad.end());
            current_block = front_block;
          }
          if (z == CHUNK_SIZE - 1 && current_block.material != Material::Air) {
            auto quad = getFace(chunk, current_block, {x, y, z},
                                BlockSide::Back, glm::vec3(1.0f));
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (x == 0 && current_block.material != Material::Air) {
            auto quad = getFace(chunk, current_block, {x, y, z},
                                BlockSide::Right, glm::vec3(1.0f));
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (x == CHUNK_SIZE - 1 && current_block.material != Material::Air) {
            auto quad = getFace(chunk, current_block, {x, y, z},
                                BlockSide::Left, glm::vec3(1.0f));
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (y == 0 && current_block.material != Material::Air) {
            auto quad = getFace(chunk, current_block, {x, y, z},
                                BlockSide::Bottom, glm::vec3(1.0f));
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (y == CHUNK_HEIGHT - 1 &&
              current_block.material != Material::Air) {
            auto quad = getFace(chunk, current_block, {x, y, z}, BlockSide::Up,
                                glm::vec3(1.0f));
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (y == ((model_id + 1) * MODEL_HEIGHT) - 1 &&
              current_block.material != Material::Air) {
            auto quad = getFace(chunk, current_block, {x, y, z}, BlockSide::Up,
                                glm::vec3(1.0f));
            vertices.insert(vertices.end(), quad.begin(), quad.end());
          }
          if (current_block.material == Material::Air) {
            glm::ivec3 positions[4] = {
                glm::ivec3(x - 1, y, z), glm::ivec3(x + 1, y, z),
                glm::ivec3(x, y + 1, z), glm::ivec3(x, y - 1, z)};
            for (int f = 0; f < 4; f++) {
              Block b = get_block(chunk->data, positions[f]);
              if (b.material != Material::Air) {
                auto quad =
                    getFace(chunk, b, positions[f], sides[f], glm::vec3(1.0f));
                vertices.insert(vertices.end(), quad.begin(), quad.end());
              }
            }
          }
        }
      }
    }
    chunk->dirty[model_id] = false;
    total_vertices += vertices.size();
    if (vertices.size() > 0) {
      if (render_attrib.vaos.size() <= model_id) {
        render_attrib.vaos.push_back(new VAO(vertices));
      } else {
        render_attrib.vaos[model_id]->update(vertices);
      }
    }
  }
}

void get_aabb(Block *data, glm::vec3 &aabb_center, glm::vec3 &aabb_halfsize,
              const glm::vec3 chunk_pos) {
  glm::ivec3 aabb_min;
  glm::ivec3 aabb_max;
  if (aabb_center.x == 0.0f && aabb_center.y == 0.0f && aabb_center.z == 0.0f) {
    int center_pos = CHUNK_SIZE / 2;
    int center_height = (MODEL_HEIGHT / 2);
    aabb_min = glm::ivec3(center_pos, center_height, center_pos);
    aabb_max = glm::ivec3(center_pos, center_height, center_pos);
  } else {
    aabb_min = (aabb_center - aabb_halfsize) - chunk_pos;
    aabb_max = (aabb_center + aabb_halfsize) - chunk_pos;
  }
  for (int y = 0; y < CHUNK_HEIGHT; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      for (int z = 0; z < CHUNK_SIZE; z++) {
        Block block = get_block(data, {x, y, z});
        if (block.material != Material::Air) {
          glm::ivec3 pos = {x, y, z};
          if (pos.x < aabb_min.x) aabb_min.x = pos.x;
          if (pos.x > aabb_max.x) aabb_max.x = pos.x;
          if (pos.y < aabb_min.y) aabb_min.y = pos.y;
          if (pos.y > aabb_max.y) aabb_max.y = pos.y;
          if (pos.z < aabb_min.z) aabb_min.z = pos.z;
          if (pos.z > aabb_max.z) aabb_max.z = pos.z;
        }
      }
    }
  }
  glm::vec3 faabb_min = glm::vec3(aabb_min);
  glm::vec3 faabb_max = glm::vec3(aabb_max);
  aabb_center = ((faabb_min + faabb_max) * 0.5f) + glm::vec3(chunk_pos);
  aabb_halfsize = (faabb_max - faabb_min) * 0.5f;
}

inline void set_block(Block *data, Block block, glm::ivec3 index) {
  data[index.y * CHUNK_SIZE * CHUNK_SIZE + index.x * CHUNK_SIZE + index.z] =
      block;
}

inline Block get_block(Block *data, glm::ivec3 index) {
  if (index.x < 0 || index.x >= CHUNK_SIZE || index.y < 0 || index.y >= 256 ||
      index.z < 0 || index.z >= CHUNK_SIZE) {
    Block block = {};
    return (block);
  }
  return (
      data[index.y * CHUNK_SIZE * CHUNK_SIZE + index.x * CHUNK_SIZE + index.z]);
}
}  // namespace mesher
