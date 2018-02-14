#pragma once
#include <algorithm>
#include <vector>
#include "env.hpp"
#include "ft_vox.hpp"

struct VAO {
  VAO(const std::vector<Vertex>& vertices);
  VAO(const std::vector<glm::vec3>& positions);
  ~VAO();
  void update(const std::vector<glm::vec3>& positions);
  GLuint vao;
  GLsizei vertices_size;
  GLsizei indices_size;

 private:
  GLuint _vbo;
};
