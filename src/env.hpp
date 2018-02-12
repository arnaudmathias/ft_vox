#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
// --
#include <GLFW/glfw3.h>
#include <array>
#include <iostream>
#include <sstream>

class InputHandler {
 public:
  int screenWidth;
  int screenHeight;
  std::array<bool, 1024> keys = {0};
  bool mouseDisabled = true;
  bool keybrDisabled = true;
  float mousex;
  float mousey;
};

class Env {
 public:
  GLFWwindow *window;
  InputHandler inputHandler;
  Env(int width, int height);
  ~Env();

  void update();  // Called once per frame
  float getDeltaTime();
  float getAbsoluteTime();
  float getFrame();
  int width;
  int height;

 private:
  void updateFpsCounter();
  void setDeltaTime();
  float _absoluteTime;
  float _deltaTime;
  float _frame;
};

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);

static void GL_DUMP_ERROR(std::string message) {
  int glErrorCode = 0;
  if ((glErrorCode = glGetError()) != GL_NO_ERROR)
    std::cout << message << " (error code: " << glErrorCode << ")\n";
}