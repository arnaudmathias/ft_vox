#pragma once
#if defined(__APPLE__) || defined(__linux__)
#include <unistd.h>
#elif defined(_WIN32)
#include <direct.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <string>
#include "ft_vox.hpp"

namespace io {
bool exists(std::string filename);
void makedir(std::string filename);

size_t encodeRLE(const Block* data, unsigned char* dest);
void decodeRLE(unsigned char* encoded_data, size_t rle_size, Block* data);

void initRegionFile(std::string filename);
void writeRegionFile(std::string filename, glm::ivec2 pos, const Block* data);
bool readRegionFile(std::string filename, glm::ivec2 pos, Block* data);
}  // namespace io
