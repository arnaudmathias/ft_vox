#include "generator.hpp"

float noise3D(glm::vec3 p) {
  float dummy;
  return std::modf(cos(glm::dot(p, glm::vec3(12.9898f, 78.233f, 128.852f))) *
                       43758.5453f,
                   &dummy) *
             2.0f -
         1.0f;
}

float simplex3D(glm::vec3 p) {
  float f3 = 1.0f / 3.0f;
  float s = (p.x + p.y + p.z) * f3;
  int i = int(floor(p.x + s));
  int j = int(floor(p.y + s));
  int k = int(floor(p.z + s));

  float g3 = 1.0 / 6.0;
  float t = float((i + j + k)) * g3;
  float x0 = float(i) - t;
  float y0 = float(j) - t;
  float z0 = float(k) - t;
  x0 = p.x - x0;
  y0 = p.y - y0;
  z0 = p.z - z0;

  int i1, j1, k1;
  int i2, j2, k2;

  if (x0 >= y0) {
    if (y0 >= z0) {
      i1 = 1;
      j1 = 0;
      k1 = 0;
      i2 = 1;
      j2 = 1;
      k2 = 0;
    }  // X Y Z order
    else if (x0 >= z0) {
      i1 = 1;
      j1 = 0;
      k1 = 0;
      i2 = 1;
      j2 = 0;
      k2 = 1;
    }  // X Z Y order
    else {
      i1 = 0;
      j1 = 0;
      k1 = 1;
      i2 = 1;
      j2 = 0;
      k2 = 1;
    }  // Z X Z order
  } else {
    if (y0 < z0) {
      i1 = 0;
      j1 = 0;
      k1 = 1;
      i2 = 0;
      j2 = 1;
      k2 = 1;
    }  // Z Y X order
    else if (x0 < z0) {
      i1 = 0;
      j1 = 1;
      k1 = 0;
      i2 = 0;
      j2 = 1;
      k2 = 1;
    }  // Y Z X order
    else {
      i1 = 0;
      j1 = 1;
      k1 = 0;
      i2 = 1;
      j2 = 1;
      k2 = 0;
    }  // Y X Z order
  }

  float x1 = x0 - float(i1) + g3;
  float y1 = y0 - float(j1) + g3;
  float z1 = z0 - float(k1) + g3;
  float x2 = x0 - float(i2) + 2.0f * g3;
  float y2 = y0 - float(j2) + 2.0f * g3;
  float z2 = z0 - float(k2) + 2.0f * g3;
  float x3 = x0 - 1.0f + 3.0f * g3;
  float y3 = y0 - 1.0f + 3.0f * g3;
  float z3 = z0 - 1.0f + 3.0f * g3;

  glm::vec3 ijk0 = glm::vec3(i, j, k);
  glm::vec3 ijk1 = glm::vec3(i + i1, j + j1, k + k1);
  glm::vec3 ijk2 = glm::vec3(i + i2, j + j2, k + k2);
  glm::vec3 ijk3 = glm::vec3(i + 1, j + 1, k + 1);

  glm::vec3 gr0 = glm::normalize(
      glm::vec3(noise3D(ijk0), noise3D(ijk0 * 2.01f), noise3D(ijk0 * 2.02f)));
  glm::vec3 gr1 = glm::normalize(
      glm::vec3(noise3D(ijk1), noise3D(ijk1 * 2.01f), noise3D(ijk1 * 2.02f)));
  glm::vec3 gr2 = glm::normalize(
      glm::vec3(noise3D(ijk2), noise3D(ijk2 * 2.01f), noise3D(ijk2 * 2.02f)));
  glm::vec3 gr3 = glm::normalize(
      glm::vec3(noise3D(ijk3), noise3D(ijk3 * 2.01f), noise3D(ijk3 * 2.02f)));

  float n0 = 0.0f;
  float n1 = 0.0f;
  float n2 = 0.0f;
  float n3 = 0.0f;

  float t0 = 0.5f - x0 * x0 - y0 * y0 - z0 * z0;
  if (t0 >= 0.0f) {
    t0 *= t0;
    n0 = t0 * t0 * glm::dot(gr0, glm::vec3(x0, y0, z0));
  }
  float t1 = 0.5f - x1 * x1 - y1 * y1 - z1 * z1;
  if (t1 >= 0.0f) {
    t1 *= t1;
    n1 = t1 * t1 * glm::dot(gr1, glm::vec3(x1, y1, z1));
  }
  float t2 = 0.5f - x2 * x2 - y2 * y2 - z2 * z2;
  if (t2 >= 0.0f) {
    t2 *= t2;
    n2 = t2 * t2 * glm::dot(gr2, glm::vec3(x2, y2, z2));
  }
  float t3 = 0.5f - x3 * x3 - y3 * y3 - z3 * z3;
  if (t3 >= 0.0f) {
    t3 *= t3;
    n3 = t3 * t3 * glm::dot(gr3, glm::vec3(x3, y3, z3));
  }
  return 96.0f * (n0 + n1 + n2 + n3);
}

float fbm(glm::vec3 st) {
  st *= 0.05;
  const int OCTAVES = 2;
  float value = 0.0f;
  float amplitude = 0.05f;
  float frequency = 0.0f;
  for (int i = 0; i < OCTAVES; i++) {
    value += amplitude * simplex3D(st);
    st *= 1.5f;
    amplitude *= 0.7f;
  }
  return value;
}
