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
  std::array<bool, 1024> keys = {{0}};
  bool mouseDisabled = true;
  bool keybrDisabled = true;
  float mousex;
  float mousey;
};

class Env {
 public:
  GLFWwindow *window;
  InputHandler inputHandler;
  Env();
  Env(unsigned short width, unsigned short height);
  ~Env();

  void update();  // Called once per frame
  float getDeltaTime();
  float getAbsoluteTime();
  float getFrame();
  float getFPS();
  int width;
  int height;

 private:
  void updateFpsCounter();
  void setDeltaTime();
  float _absoluteTime;
  float _deltaTime;
  float _frame;
  float _fps;
};

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);

static void GL_DUMP_ERROR(std::string message) {
  int glErrorCode = 0;
  if ((glErrorCode = glGetError()) != GL_NO_ERROR) {
    std::string error;
    switch (glErrorCode) {
      case GL_INVALID_ENUM:
        error = "INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        error = "INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:
        error = "INVALID_OPERATION";
        break;
      case GL_STACK_OVERFLOW:
        error = "STACK_OVERFLOW";
        break;
      case GL_STACK_UNDERFLOW:
        error = "STACK_UNDERFLOW";
        break;
      case GL_OUT_OF_MEMORY:
        error = "OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        error = "INVALID_FRAMEBUFFER_OPERATION";
        break;
    }
    std::cout << message << " (" << glErrorCode << ": " << error << ")\n";
  }
}
