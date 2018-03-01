#include "io.hpp"

namespace io {
bool exists(std::string filename) {
#if defined(__APPLE__) && defined(__linux__)
  struct stat st = {0};
  if (stat(filename.c_str(), &st) != -1) {
    return (true);
  }
#elif defined(_WIN32)
#endif
  return (false);
}
void makedir(std::string filename) { mkdir(filename.c_str(), 0700); }

size_t encodeRLE(const Block* data, unsigned char* dest) {
  size_t len_rle = 0;
  for (size_t i = 0; i < (CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT); i++) {
    unsigned char c = 1;
    while (data[i] == data[i + 1] && c < 255) {
      c++;
      i++;
    }
    dest[len_rle++] = c;
    dest[len_rle++] = static_cast<unsigned char>(data[i].material);
  }
  return (len_rle);
}

void decodeRLE(unsigned char* encoded_data, size_t rle_size, Block* data) {
  for (int i = 0; i < rle_size; i++) {
    if (encoded_data[i] == 0) {
      return;  // EOF
    }
    int c = encoded_data[i];
    char value = encoded_data[i++];
    for (int j = 0; j < c; j++) {
      data[i].material = static_cast<enum Material>(encoded_data[i + j]);
    }
  }
}

void readRegionFile(std::string filename, glm::ivec2 pos, Block* data) {
  unsigned char chunk_rle[(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT) * 2] = {0};
  unsigned char lookup[REGION_LOOKUPTABLE_SIZE];
  FILE* region = fopen(filename.c_str(), "r");
  if (region != NULL) {
    fread(lookup, REGION_LOOKUPTABLE_SIZE, 1, region);
    pos /= REGION_SIZE;
    int lookup_offset = 4 * ((abs(pos.x) % REGION_SIZE) +
                             (abs(pos.y) % REGION_SIZE) * REGION_SIZE);
    unsigned int offset = ((unsigned int)(lookup[lookup_offset + 0]) << 16) |
                          ((unsigned int)(lookup[lookup_offset + 1]) << 8) |
                          ((unsigned int)(lookup[lookup_offset + 2]));
    unsigned int sector_count = (lookup[lookup_offset + 3] & 0xff);
    fseek(region, offset, SEEK_SET);
    fread(chunk_rle, sector_count * CHUNK_OFFSET, 1, region);
    decodeRLE(chunk_rle, sector_count * CHUNK_OFFSET, data);
    fclose(region);
  }
}

void initRegionFile(std::string filename) {
  unsigned char lookup[REGION_LOOKUPTABLE_SIZE] = {0};
  unsigned int offset = 0;
  for (int i = 0; i < REGION_LOOKUPTABLE_SIZE; i += 4) {
    lookup[i + 0] = (offset & 0xff0000) >> 16;
    lookup[i + 1] = (offset & 0xff00) >> 8;
    lookup[i + 2] = (offset & 0xff);
    lookup[i + 3] = (1 & 0xff);
    offset += CHUNK_OFFSET;
  }
  FILE* region = fopen(filename.c_str(), "w+");
  if (region != NULL) {
    fwrite(lookup, REGION_LOOKUPTABLE_SIZE, 1, region);
    fclose(region);
  }
}

void writeRegionFile(std::string filename, glm::ivec2 pos, const Block* data) {
  unsigned char lookup[REGION_LOOKUPTABLE_SIZE] = {0};
  unsigned char chunk_rle[(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT) * 2] = {0};

  if (io::exists(filename) == false) {
    initRegionFile(filename);
  }
  size_t len_rle = encodeRLE(data, chunk_rle);
  FILE* region = fopen(filename.c_str(), "r+");
  if (region != NULL) {
    fread(lookup, REGION_LOOKUPTABLE_SIZE, 1, region);
    pos /= REGION_SIZE;
    int lookup_offset = 4 * ((abs(pos.x) % REGION_SIZE) +
                             (abs(pos.y) % REGION_SIZE) * REGION_SIZE);
    unsigned int offset = ((unsigned int)(lookup[lookup_offset + 0]) << 16) |
                          ((unsigned int)(lookup[lookup_offset + 1]) << 8) |
                          ((unsigned int)(lookup[lookup_offset + 2]));
    unsigned int sector_count = (lookup[lookup_offset + 3] & 0xff);
    // std::cout << "offset: " << offset << "|" << sector_count << std::endl;
    fseek(region, offset, SEEK_SET);
    fwrite(chunk_rle, len_rle, 1, region);
    fclose(region);
  }
}

}  // namespace io
