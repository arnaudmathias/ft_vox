#pragma once
#include <vector>
#include "ft_vox.hpp"
#include "renderer.hpp"

namespace mesher {
void greedy(Block *data, bool *dirty, RenderAttrib &render_attrib);
void culling(Block *data, bool *dirty, RenderAttrib &render_attrib);
void get_aabb(Block *data, glm::vec3 &aabb_center, glm::vec3 &aabb_halfsize,
              const glm::vec3 chunk_pos);
void set_block(Block *data, Block block, glm::ivec3 index);
Block get_block(Block *data, glm::ivec3 index);
}  // namespace mesher
