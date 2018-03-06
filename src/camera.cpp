#include "camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 targetPosition, int width,
               int height)
    : pos(position),
      width(width),
      height(height),
      mouseInit(false),
      mouseMoved(false),
      speed(1.0f) {
  glm::vec3 direction = glm::normalize(targetPosition - position);
  verAngle = asinf(direction.y);
  horAngle = atan2(direction.x, direction.z);
  zNear = 0.1f;
  zFar = 1000.0f;
  proj = glm::perspective(
      glm::radians(80.0f),
      static_cast<float>(width) / static_cast<float>(height), zNear, zFar);
  fovYscale = tan(glm::radians(80.0f) / 2.0f);
  update();
}

void Camera::update() {
  double currentTime = glfwGetTime();
  deltaTime = static_cast<float>(currentTime - lastTime);
  lastTime = static_cast<float>(currentTime);
  if (mouseMoved) {
    horAngle += 0.2f * deltaTime * (oldMouseXpos - mouseXpos);
    verAngle += 0.2f * deltaTime * (oldMouseYpos - mouseYpos);
    verAngle = glm::clamp(verAngle, -1.5f, 1.5f);
    mouseMoved = false;
  }
  dir = glm::vec3(cos(verAngle) * sin(horAngle), sin(verAngle),
                  cos(verAngle) * cos(horAngle));
  dir = glm::normalize(dir);
  right =
      glm::vec3(sin(horAngle - 3.14f / 2.0f), 0, cos(horAngle - 3.14f / 2.0f));
  right = glm::normalize(right);
  up = glm::normalize(glm::cross(right, dir));
  view = glm::lookAt(pos, dir + pos, up);
  if (static_cast<float>(currentTime - lastVelocity) > 1.0f) {
    velocity = glm::distance(lastPos, pos) /
               (static_cast<float>(currentTime) - lastVelocity);
    lastVelocity = static_cast<float>(currentTime);
    lastPos = pos;
  }
}

void Camera::rotate(float hor, float ver) {
  horAngle += hor;
  verAngle += ver;
}

void Camera::queryInput(std::array<bool, 1024> keys, float mouse_x,
                        float mouse_y) {
  if (keys[GLFW_KEY_LEFT_SHIFT]) {
    speed = 20.0f;
  } else {
    speed = 5.0f;
  }
  if (keys[GLFW_KEY_UP] || keys[GLFW_KEY_W]) {
    glm::vec3 tmp = dir * speed * deltaTime;
    pos = pos + tmp;
  }
  if (keys[GLFW_KEY_DOWN] || keys[GLFW_KEY_S]) {
    glm::vec3 tmp = dir * speed * deltaTime;
    pos = pos - tmp;
  }
  if (keys[GLFW_KEY_RIGHT] || keys[GLFW_KEY_D]) {
    glm::vec3 right = glm::cross(up, dir);
    glm::vec3 tmp = right * speed * deltaTime;
    pos = pos - tmp;
  }
  if (keys[GLFW_KEY_LEFT] || keys[GLFW_KEY_A]) {
    glm::vec3 right = glm::cross(up, dir);
    glm::vec3 tmp = right * speed * deltaTime;
    pos = pos + tmp;
  }
  if (mouse_x != mouseXpos || mouse_y != mouseYpos) {
    if (mouseInit) {
      oldMouseXpos = mouseXpos;
      oldMouseYpos = mouseYpos;
    } else {
      oldMouseXpos = mouse_x;
      oldMouseYpos = mouse_y;
      mouseInit = true;
    }
    mouseXpos = mouse_x;
    mouseYpos = mouse_y;
    this->mouseMoved = true;
  }
}

float Camera::getAspectRatio() {
  return (static_cast<float>(width) / static_cast<float>(height));
}
