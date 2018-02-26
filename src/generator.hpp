#pragma once
#include <iostream>
#include "ft_vox.hpp"

namespace generator {

void generate_chunk(Block *data, glm::vec3 chunk_pos);
float fbm(glm::vec3 st);
void set_block(Block *data, Block block, glm::ivec3 index);

}  // namespace generator
