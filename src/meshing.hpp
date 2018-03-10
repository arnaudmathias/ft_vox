#pragma once
#include <vector>
#include "chunk.hpp"
#include "ft_vox.hpp"
#include "renderer.hpp"

class Chunk;

namespace mesher {
void greedy(Chunk *chunk, RenderAttrib &render_attrib);
void culling(Chunk *chunk, RenderAttrib &render_attrib);
void get_aabb(Block *data, glm::vec3 &aabb_center, glm::vec3 &aabb_halfsize,
              const glm::vec3 chunk_pos);
glm::ivec3 get_interval(Block *data, glm::ivec3 pos, Block current_block);
void set_block(Block *data, Block block, glm::ivec3 index);
Block get_block(Block *data, glm::ivec3 index);
	glm::vec3 get_normal(enum BlockSide side);
}  // namespace mesher
