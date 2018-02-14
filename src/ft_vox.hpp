#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

struct Vertex {
  glm::vec3 position = {0, 0, 0};
  unsigned int data = {0};
  Vertex() : position({0.0f, 0.0f, 0.0f}), data(0){};
  Vertex(Vertex const& src) { *this = src; }
  Vertex& operator=(Vertex const& rhs) {
    if (this != &rhs) {
      this->position = rhs.position;
      this->data = rhs.data;
    }
    return (*this);
  };
};
