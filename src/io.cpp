#include "io.hpp"

namespace io {
bool exists(std::string filename) {
  struct stat st = {0};
  if (stat(filename.c_str(), &st) != -1) {
    return (true);
  }
  return (false);
}
void makedir(std::string filename) {
#if defined(__APPLE__) || defined(__linux__)
  mkdir(filename.c_str(), 0700);
#elif defined(_WIN32)
  _mkdir(filename.c_str());
#endif
}
unsigned int get_filesize(std::string filename) {
  struct stat st = {0};
  if (stat(filename.c_str(), &st) != -1) {
    return (st.st_size);
  }
  return (0);
}

size_t encodeRLE(const Block* data, unsigned char* dest) {
  size_t len_rle = 0;
  for (size_t i = 0; i < (CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT); i++) {
    unsigned char c = 1;
    while (data[i] == data[i + 1] && c < 255) {
      c++;
      i++;
    }
    dest[len_rle] = c;
    dest[len_rle + 1] = static_cast<unsigned char>(data[i].material);
    len_rle += 2;
  }
  return (len_rle - 2);
}

void decodeRLE(unsigned char* encoded_data, size_t rle_size, Block* data,
               unsigned int limit) {
  size_t data_offset = 0;
  for (unsigned int i = 0; i < rle_size; i += 2) {
    unsigned char len = encoded_data[i];
    unsigned char value = encoded_data[i + 1];
    for (int j = 0; j < len; j++) {
      if (data_offset >= limit) return;
      data[data_offset].material = static_cast<Material>(value);
      data_offset++;
    }
  }
}

void initRegionFile(std::string filename) {
  unsigned char lookup[REGION_LOOKUPTABLE_SIZE] = {0};
  FILE* region = fopen(filename.c_str(), "w+b");
  if (region != NULL) {
    fwrite(lookup, REGION_LOOKUPTABLE_SIZE, 1, region);
    fclose(region);
  }
}

}  // namespace io
