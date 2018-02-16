#include <list>
#include "camera.hpp"
#include "chunk.hpp"
#include "env.hpp"
#include "renderer.hpp"
#include "shader.hpp"

int main(int argc, char **argv) {
  Env env(1280, 720);
  ChunkManager chunkManager;
  Renderer renderer(1280, 720);
  Camera camera(glm::vec3(0.0, 70.0, 1.0), glm::vec3(0.0, 70.0, 0.0), 1280,
                720);
  bool wireframe = false;
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  while (!glfwWindowShouldClose(env.window)) {
    env.update();
    chunkManager.update(camera.pos);
    chunkManager.setRenderAttributes(renderer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();
    camera.queryInput(env.inputHandler.keys, env.inputHandler.mousex,
                      env.inputHandler.mousey);
    camera.update();
    renderer.uniforms.view = camera.view;
    renderer.uniforms.proj = camera.proj;
    renderer.draw();
    renderer.flush();
    glfwSwapBuffers(env.window);
    GL_DUMP_ERROR("draw loop");
    if (env.inputHandler.keys[GLFW_KEY_ESCAPE]) {
      glfwSetWindowShouldClose(env.window, 1);
    }
    if (env.inputHandler.keys[GLFW_KEY_M]) {
      env.inputHandler.keys[GLFW_KEY_M] = false;
      wireframe = !wireframe;
      wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
                : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
  }
}
