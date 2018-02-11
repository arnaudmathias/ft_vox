#include <list>
#include "env.hpp"

int main(int argc, char **argv) {
  Env env(1280, 720);

  while (!glfwWindowShouldClose(env.window)) {
    env.update();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();

    glfwSwapBuffers(env.window);
    GL_DUMP_ERROR("draw loop");
    if (env.inputHandler.keys[GLFW_KEY_ESCAPE]) {
      glfwSetWindowShouldClose(env.window, 1);
    }
  }
}
