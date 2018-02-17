#pragma once
#include "ft_vox.hpp"

class FrustrumCulling {
 public:
  FrustrumCulling(void);
  FrustrumCulling(FrustrumCulling const& src);
  ~FrustrumCulling(void);
  FrustrumCulling& operator=(FrustrumCulling const& rhs);
  void updateProjView(const glm::mat4& proj_view);
  void cull(size_t count, const glm::vec3* centers, const glm::vec3* halfsizes,
            unsigned char* results);

 private:
  glm::vec4 frustrum_planes[6];
};
