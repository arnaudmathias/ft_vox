#include "generator.hpp"

namespace generator {

std::vector<int> permutation;

inline float noise2D(uint32_t seed, const glm::vec2 &v) {
  uint32_t s = static_cast<uint32_t>(v.x) * 1087;
  s ^= 0xE56FAA12;
  s += static_cast<uint32_t>(v.y) * 2749;
  s ^= 0x69628a2d;
  return (static_cast<float>(static_cast<int>(s % 2001) - 1000) / 1000.0f);
}

inline float noise3D(uint32_t seed, const glm::vec3 &v) {
  uint32_t s = static_cast<uint32_t>(v.x) * 1087;
  s ^= 0xE56FAA12;
  s += static_cast<uint32_t>(v.y) * 2749;
  s ^= 0x69628a2d;
  s += static_cast<uint32_t>(v.z) * 3433;
  s ^= 0xa7b2c49a;
  return (static_cast<float>(static_cast<int>(s % 2001) - 1000) / 1000.0f) *
             0.5 +
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
  double u = h < 8 ? x : y, v = h < 4 ? y : h == 12 || h == 14 ? x : z;
  return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

int inc(int num) {
  num++;
  num %= 512;
  return num;
}

float gradientNoise3D(uint32_t seed, glm::vec3 pos) {
  glm::vec3 p = pos;
  p.x = fmod(fabs(pos.x), 512.0f);
  p.y = fmod(fabs(pos.y), 512.0f);
  p.z = fmod(fabs(pos.z), 512.0f);
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

  return (lerp(y1, y2, w) + 1) / 2;

  /*
  float v000 = noise3D(seed, glm::vec3(xi, yi, zi));
  float v001 = noise3D(seed, glm::vec3(xi, yi, zi + 1.0f));
  float v010 = noise3D(seed, glm::vec3(xi, yi + 1.0f, zi));
  float v011 = noise3D(seed, glm::vec3(xi, yi + 1.0f, zi + 1.0f));
  float v100 = noise3D(seed, glm::vec3(xi + 1.0f, yi, zi));
  float v101 = noise3D(seed, glm::vec3(xi + 1.0f, yi, zi + 1.0f));
  float v110 = noise3D(seed, glm::vec3(xi + 1.0f, yi + 1.0f, zi));
  float v111 = noise3D(seed, glm::vec3(xi + 1.0f, yi + 1.0f, zi + 1.0f));
  return (trilinear_lerp(v000, v001, v010, v011, v100, v101, v110, v111, xf, yf,
                         zf));*/
}

float gradientNoise2D(uint32_t seed, const glm::vec2 &pos) {
  int xi = floor(pos.x);
  int yi = floor(pos.y);
  float xf = glm::fract(pos.x);
  float yf = glm::fract(pos.y);
  float v00 = noise2D(seed, glm::vec2(xi, yi));
  float v01 = noise2D(seed, glm::vec2(xi, yi + 1.0f));
  float v10 = noise2D(seed, glm::vec2(xi + 1.0f, yi));
  float v11 = noise2D(seed, glm::vec2(xi + 1.0f, yi + 1.0f));
  return (bilinear_lerp(v00, v10, v01, v11, xf, yf));
}

float perlin3D(uint32_t seed, glm::vec3 v, const int octaves,
               float persistence) {
  v *= 0.01;
  float value = 0.0f;
  float amplitude = 1.0f;
  float frequency = 1.0f;
  float total_amplitude = 0.0f;
  for (int i = 0; i < octaves; i++) {
    value += amplitude * gradientNoise3D(seed, v * frequency);
    total_amplitude += amplitude;
    amplitude *= persistence;
    frequency *= 2.0f;
  }
  return (value / total_amplitude);
}

float perlin2D(uint32_t seed, glm::vec2 v, const int octaves,
               float persistence) {
  v *= 0.01;
  float value = 0.0f;
  float amplitude = 1.0f;
  float frequency = 1.0f;
  float total_amplitude = 0.0f;
  for (int i = 0; i < octaves; i++) {
    value += amplitude * gradientNoise2D(seed, v * frequency);
    total_amplitude += amplitude;
    amplitude *= persistence;
    frequency *= 2.0f;
  }
  return (value / total_amplitude);
}

void generate_chunk(Block *data, glm::vec3 pos) {
  for (int x = 0; x < 16; x++) {
    for (int z = 0; z < 16; z++) {
      float h_map = perlin3D(0, glm::vec3(pos.x + x, pos.z + z, 0.0f), 6, 0.5f);
      int height = round(64.0f * h_map);
      for (int y = 0; y < height; y++) {
        Block block;
        if (y == height - 1) {
          if (y < 67) {
            block.material = Material::Dirt;
          } else {
            block.material = Material::Stone;
          }
        } else {
          block.material = Material::Dirt;
        }
        /*float h_cave = fbm(glm::vec3(pos.x + x, pos.y + y, pos.z + z),
                           glm::vec3(0.05f, 0.05f, 0.05f));
        if (h_cave < 0.0) {*/
        set_block(data, block, glm::ivec3(x, y, z));
        //}
      }
    }
  }
}

inline void set_block(Block *data, Block block, glm::ivec3 index) {
  data[index.y * CHUNK_SIZE * CHUNK_SIZE + index.x * CHUNK_SIZE + index.z] =
      block;
}

void init(uint32_t seed) {
  permutation.resize(256);
  std::iota(permutation.begin(), permutation.end(), 0);
  std::default_random_engine engine(seed);
  std::shuffle(permutation.begin(), permutation.end(), engine);
  permutation.insert(permutation.end(), permutation.begin(), permutation.end());
  std::cout << permutation.size() << std::endl;
}

}  // namespace generator
