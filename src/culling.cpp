#include "culling.hpp"

FrustrumCulling::FrustrumCulling(void) {}

FrustrumCulling::FrustrumCulling(FrustrumCulling const &src) { *this = src; }

FrustrumCulling::~FrustrumCulling(void) {}

FrustrumCulling &FrustrumCulling::operator=(FrustrumCulling const &rhs) {
  if (this != &rhs) {
  }
  return (*this);
}

void FrustrumCulling::updateProjView(const glm::mat4 &proj_view) {
  frustrum_planes[0] = glm::normalize(proj_view[0] + proj_view[3]);
  frustrum_planes[1] = glm::normalize(proj_view[3] - proj_view[0]);
  frustrum_planes[2] = glm::normalize(proj_view[3] - proj_view[1]);
  frustrum_planes[3] = glm::normalize(proj_view[3] + proj_view[1]);
  frustrum_planes[4] = glm::normalize(proj_view[2]);
  frustrum_planes[5] = glm::normalize(proj_view[3] - proj_view[2]);
}

void FrustrumCulling::cull(size_t count, const glm::vec3 *centers,
                           const glm::vec3 *halfsizes, unsigned char *results) {
  /*
  for (int i = 0; i < count; i++) {
    for (int plane_id = 0; plane_id < 6; plane_id++) {
      glm::vec3 absPlane = glm::abs(frustrum_planes[plane_id]);
      float d = dot(centers[i], frustrum_planes[plane_id]);
      float r = dot(halfsizes[i], absPlane);
      if (d + r > 0) {
      }
    }
  } */
}
