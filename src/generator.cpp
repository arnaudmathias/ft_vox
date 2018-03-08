#include "generator.hpp"

namespace generator {

const BiomeDef biome_lookup[] = {{{1.0f, 1.0f, 1.0f}}, {{1.0f, 1.0f, 1.0f}},
                                 {{1.0f, 1.0f, 1.0f}}, {{1.0f, 1.0f, 1.0f}},
                                 {{1.0f, 1.0f, 1.0f}}, {{1.0f, 1.0f, 1.0f}},
                                 {{1.0f, 1.0f, 1.0f}}};
std::vector<int> permutation;

inline float noise2D(const glm::vec2 &v) {
  uint32_t s = static_cast<uint32_t>(v.x) * 1087;
  s ^= 0xE56FAA12;
  s += static_cast<uint32_t>(v.y) * 2749;
  s ^= 0x69628a2d;
  return (static_cast<float>(static_cast<int>(s % 2001) - 1000) / 1000.0f);
}

inline float noise3D(const glm::vec3 &v) {
  uint32_t s = static_cast<uint32_t>(v.x) * 1087;
  s ^= 0xE56FAA12;
  s += static_cast<uint32_t>(v.y) * 2749;
  s ^= 0x69628a2d;
  s += static_cast<uint32_t>(v.z) * 3433;
  s ^= 0xa7b2c49a;
  return (static_cast<float>(static_cast<int>(s % 2001) - 1000) / 1000.0f) *
             0.5f +
         1.0f;
}

template <typename T>
inline float lerp(T a, T b, T x) {
  return (a + x * (b - a));
}

template <typename T>
inline float bilinear_lerp(T v00, T v10, T v01, T v11, float x, float y) {
  float a = lerp(v00, v10, x);
  float b = lerp(v10, v11, x);
  return (lerp(a, b, y));
}

template <typename T>
inline float trilinear_lerp(T v000, T v001, T v010, T v011, T v100, T v101,
                            T v110, T v111, float x, float y, float z) {
  float a = bilinear_lerp(v000, v100, v010, v110, x, y);
  float b = bilinear_lerp(v001, v101, v011, v111, x, y);
  return (lerp(a, b, z));
}
float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }

float grad(int hash, float x, float y, float z) {
  int h = hash & 15;
  float u = h < 8 ? x : y, v = h < 4 ? y : h == 12 || h == 14 ? x : z;
  return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

int inc(int num) {
  num++;
  num %= static_cast<uint32_t>(permutation.size() - 1);
  return num;
}

float gradientNoise3D(glm::vec3 pos) {
  glm::vec3 p = pos;
  p.x = fmod(fabs(pos.x), static_cast<float>(permutation.size() - 1));
  p.y = fmod(fabs(pos.y), static_cast<float>(permutation.size() - 1));
  p.z = fmod(fabs(pos.z), static_cast<float>(permutation.size() - 1));
  int xi = static_cast<int>(floor(p.x));
  int yi = static_cast<int>(floor(p.y));
  int zi = static_cast<int>(floor(p.z));
  float xf = glm::fract(p.x);
  float yf = glm::fract(p.y);
  float zf = glm::fract(p.z);
  float u = fade(xf);
  float v = fade(yf);
  float w = fade(zf);
  int aaa, aba, aab, abb, baa, bba, bab, bbb;
  aaa = permutation[permutation[permutation[xi] + yi] + zi];
  aba = permutation[permutation[permutation[xi] + inc(yi)] + zi];
  aab = permutation[permutation[permutation[xi] + yi] + inc(zi)];
  abb = permutation[permutation[permutation[xi] + inc(yi)] + inc(zi)];
  baa = permutation[permutation[permutation[inc(xi)] + yi] + zi];
  bba = permutation[permutation[permutation[inc(xi)] + inc(yi)] + zi];
  bab = permutation[permutation[permutation[inc(xi)] + yi] + inc(zi)];
  bbb = permutation[permutation[permutation[inc(xi)] + inc(yi)] + inc(zi)];
  float x1, x2, y1, y2;
  x1 = lerp(grad(aaa, xf, yf, zf), grad(baa, xf - 1, yf, zf), u);
  x2 = lerp(grad(aba, xf, yf - 1, zf), grad(bba, xf - 1, yf - 1, zf), u);
  y1 = lerp(x1, x2, v);
  x1 = lerp(grad(aab, xf, yf, zf - 1), grad(bab, xf - 1, yf, zf - 1), u);
  x2 =
      lerp(grad(abb, xf, yf - 1, zf - 1), grad(bbb, xf - 1, yf - 1, zf - 1), u);
  y2 = lerp(x1, x2, v);

  return (lerp(y1, y2, w) + 1.0f) / 2.0f;
}

float gradientNoise2D(const glm::vec2 &pos) {
  glm::vec2 p = pos;
  p.x = fmod(fabs(pos.x), static_cast<float>(permutation.size() - 1));
  p.y = fmod(fabs(pos.y), static_cast<float>(permutation.size() - 1));
  int xi = static_cast<int>(floor(p.x));
  int yi = static_cast<int>(floor(p.y));
  float xf = glm::fract(p.x);
  float yf = glm::fract(p.y);
  float u = fade(xf);
  float v = fade(yf);
  int aa, ab, ba, bb;
  aa = permutation[permutation[xi] + yi];
  ab = permutation[permutation[xi] + inc(yi)];
  ba = permutation[permutation[inc(xi)] + yi];
  bb = permutation[permutation[inc(xi)] + inc(yi)];
  float x1, x2;
  x1 = lerp(grad(aa, xf, yf, 0.0f), grad(ba, xf - 1, yf, 0.0f), u);
  x2 = lerp(grad(ab, xf, yf - 1, 0.0f), grad(bb, xf - 1, yf - 1, 0.0f), u);
  return ((lerp(x1, x2, v) + 1.0f)) / 2.0f;
}

float ridgedNoise3D(const glm::vec3 &pos) {
  return (2.0f * (0.5f - fabs(0.5f - gradientNoise3D(pos))));
  // return ((fabs(0.5f - gradientNoise2D(pos))));
}

float ridgedNoise2D(const glm::vec2 &pos) {
  return (2.0f * (0.5f - fabs(0.5f - gradientNoise2D(pos))));
  // return ((fabs(0.5f - gradientNoise2D(pos))));
}

float perlin3D(glm::vec3 v, const int octaves, float persistence,
               float frequency, glm::vec3 scale) {
  v *= scale;
  float value = 0.0f;
  float amplitude = 1.0f;
  float total_amplitude = 0.0f;
  for (int i = 0; i < octaves; i++) {
    value += amplitude * gradientNoise3D(v * frequency);
    total_amplitude += amplitude;
    amplitude *= persistence;
    frequency *= 2.0f;
  }
  return (value / total_amplitude);
}

float perlin2D(glm::vec2 v, const int octaves, float persistence,
               float frequency, glm::vec2 scale) {
  v *= scale;
  float value = 0.0f;
  float amplitude = 1.0f;
  float total_amplitude = 0.0f;
  for (int i = 0; i < octaves; i++) {
    value += amplitude * gradientNoise2D(v * frequency);
    total_amplitude += amplitude;
    amplitude *= persistence;
    frequency *= 2.0f;
  }
  return (value / total_amplitude);
}
float ridged3D(glm::vec3 v, const int octaves, float persistence,
               float frequency, glm::vec3 scale) {
  v *= scale;
  float value = 0.0f;
  float amplitude = 1.0f;
  float total_amplitude = 0.0f;
  for (int i = 0; i < octaves; i++) {
    value += amplitude * ridgedNoise2D(v * frequency);
    total_amplitude += amplitude;
    amplitude *= persistence;
    frequency *= 2.0f;
  }
  return (value / total_amplitude);
}

float ridged2D(glm::vec2 v, const int octaves, float persistence,
               float frequency, glm::vec2 scale) {
  v *= scale;
  float value = 0.0f;
  float amplitude = 1.0f;
  float total_amplitude = 0.0f;
  for (int i = 0; i < octaves; i++) {
    value += amplitude * ridgedNoise2D(v * frequency);
    total_amplitude += amplitude;
    amplitude *= persistence;
    frequency *= 2.0f;
  }
  return (value / total_amplitude);
}

enum Biome get_biome(float elevation) {
  if (elevation < 0.1)
    return Biome::Water;
  else if (elevation < 0.2)
    return Biome::Beach;
  else if (elevation < 0.3)
    return Biome::Forest;
  else if (elevation < 0.5)
    return Biome::Jungle;
  else if (elevation < 0.7)
    return Biome::Savannah;
  else
    return Biome::Desert;
}

void generate_chunk(Block *data, Biome *biome_data, glm::vec3 pos) {
  pos += permutation.size() / 2;
  for (int x = 0; x < 16; x++) {
    for (int z = 0; z < 16; z++) {
      /*
      float mountain_value = ridged2D(glm::vec2(pos.x + x, pos.z + z), 20, 0.2f,
                                      0.3f, {0.005f, 0.005f});*/
      float mountain_value = perlin2D(glm::vec2(pos.x + x, pos.z + z), 20, 0.2f,
                                      0.2f, {0.07f, 0.07f});
      mountain_value = glm::pow(mountain_value, 4.0f) + 0.1f;
      // mountain_value -= 0.5f;
      float flat_base_value = perlin2D(glm::vec2(pos.x + x, pos.z + z), 5, 0.5f,
                                       2.0f, {0.01f, 0.01f});
      flat_base_value *= 0.125f;
      flat_base_value += 0.5f;
      float terrain_value = perlin2D(glm::vec2(pos.x + x, pos.z + z), 4, 0.1f,
                                     0.5f, {0.005f, 0.005f});

      float h_map = (1.0f - terrain_value) * flat_base_value +
                    (terrain_value)*mountain_value;
      Biome biome = get_biome(h_map);
      biome_data[x * CHUNK_SIZE + z] = biome;
      int height = static_cast<int>(round(256.0f * h_map));
      for (int y = 0; y < height; y++) {
        Block block;
        if (y == 0) {
          block.material = Material::Bedrock;
        } else if (y < height - 3) {
          block.material = Material::Stone;
        } else {
          if (biome == Biome::Water) {
            block.material = Material::Dirt;
          } else if (biome == Biome::Beach) {
            block.material = Material::Sand;
          } else if (biome == Biome::Forest) {
            block.material = Material::Dirt;
          } else if (biome == Biome::Jungle) {
            block.material = Material::Dirt;
          } else if (biome == Biome::Savannah) {
            block.material = Material::Dirt;
          } else {
            block.material = Material::Stone;
          }
        }
        float h_cave = perlin3D(glm::vec3(pos.x + x, pos.y + y, pos.z + z), 4,
                                0.9f, 1.0f, {0.01f, 0.004f, 0.01f});
        if (y == 0 || h_cave < 0.63) {
          set_block(data, block, glm::ivec3(x, y, z));
        }
      }
    }
  }
}

inline void set_block(Block *data, Block block, glm::ivec3 index) {
  data[index.y * CHUNK_SIZE * CHUNK_SIZE + index.x * CHUNK_SIZE + index.z] =
      block;
}

void init(uint32_t size, uint32_t seed) {
  permutation.resize(size / 2);
  std::iota(permutation.begin(), permutation.end(), 0);
  std::default_random_engine engine(seed);
  std::shuffle(permutation.begin(), permutation.end(), engine);
  permutation.insert(permutation.end(), permutation.begin(), permutation.end());
  std::cout << permutation.size() << std::endl;
}

}  // namespace generator
