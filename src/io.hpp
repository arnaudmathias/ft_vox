#pragma once
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include "ft_vox.hpp"

namespace io {
bool exists(std::string filename);
void makedir(std::string filename);

void writeRegionFile(std::string filename, glm::ivec2 pos, const Block* data);
void readRegionFile(std::string filename, Block* data);
}  // namespace io
