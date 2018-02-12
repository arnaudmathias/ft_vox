#include <list>
#include "chunk.hpp"
#include "env.hpp"
#include "renderer.hpp"
#include "shader.hpp"

int main(int argc, char **argv) {
  Env env(1280, 720);
  ChunkManager chunkManager;
  Renderer renderer(1280, 720);

  while (!glfwWindowShouldClose(env.window)) {
    env.update();
    chunkManager.setRenderAttributes(renderer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();
    renderer.draw();
    renderer.flush();
    glfwSwapBuffers(env.window);
    GL_DUMP_ERROR("draw loop");
    if (env.inputHandler.keys[GLFW_KEY_ESCAPE]) {
      glfwSetWindowShouldClose(env.window, 1);
    }
  }
}
