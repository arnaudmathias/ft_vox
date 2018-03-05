#include <iomanip>
#include <list>
#include "camera.hpp"
#include "chunk.hpp"
#include "env.hpp"
#include "renderer.hpp"
#include "shader.hpp"

std::string float_to_string(float f, int prec) {
  std::ostringstream out;
  out << std::setprecision(prec) << std::fixed << f;
  return out.str();
}

void print_debug_info(Env &env, Renderer &renderer, Camera &camera,
                      ChunkManager &cm) {
  float fheight = static_cast<float>(renderer.getScreenHeight());
  float fwidth = static_cast<float>(renderer.getScreenWidth());
  renderer.renderText(10.0f, fheight - 25.0f, 0.35f,
                      "x: " + float_to_string(camera.pos.x, 2) +
                          " y: " + float_to_string(camera.pos.y, 2) +
                          " z: " + float_to_string(camera.pos.z, 2),
                      glm::vec3(1.0f, 1.0f, 1.0f));
  renderer.renderText(10.0f, fheight - 50.0f, 0.35f,
                      "vel: " + float_to_string(camera.velocity, 2) + " m/s",
                      glm::vec3(1.0f, 1.0f, 1.0f));
  renderer.renderText(fwidth - 130.0f, fheight - 25.0f, 0.35f,
                      float_to_string(env.getFPS(), 2) + " fps",
                      glm::vec3(1.0f, 1.0f, 1.0f));
  cm.print_chunkmanager_info(renderer, fheight, fwidth);
}

int main(int argc, char **argv) {
  Env env(1280, 720);
  ChunkManager chunkManager;
  Renderer renderer(env.width, env.height);
  renderer.loadCubeMap(
      "shaders/skybox.vert", "shaders/skybox.frag",
      {"textures/skybox_side.png", "textures/skybox_side.png",
       "textures/skybox_up.png", "textures/skybox_bottom.png",
       "textures/skybox_side.png", "textures/skybox_side.png"});
  Camera camera(glm::vec3(0.0, 70.0, 1.0), glm::vec3(0.0, 70.0, 0.0), env.width,
                env.height);
  bool wireframe = false;
  bool debugMode = false;
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  while (!glfwWindowShouldClose(env.window)) {
    env.update();
    chunkManager.update(camera.pos);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();
    camera.queryInput(env.inputHandler.keys, env.inputHandler.mousex,
                      env.inputHandler.mousey);
    camera.update();
    renderer.uniforms.view = camera.view;
    renderer.uniforms.proj = camera.proj;
    renderer.uniforms.view_proj = camera.proj * camera.view;
    chunkManager.setRenderAttributes(renderer, camera.pos);
    renderer.draw();
    renderer.flush();
    if (debugMode) {
      print_debug_info(env, renderer, camera, chunkManager);
    }
    glfwSwapBuffers(env.window);
    GL_DUMP_ERROR("draw loop");
    if (env.inputHandler.keys[GLFW_KEY_ESCAPE]) {
      glfwSetWindowShouldClose(env.window, 1);
    }
    if (env.inputHandler.mouse_keys[GLFW_MOUSE_BUTTON_LEFT]) {
      env.inputHandler.mouse_keys[GLFW_MOUSE_BUTTON_LEFT] = false;
      chunkManager.rayCast(camera.dir, camera.pos);
    }
    if (env.inputHandler.keys[GLFW_KEY_M]) {
      env.inputHandler.keys[GLFW_KEY_M] = false;
      wireframe = !wireframe;
      wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
                : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (env.inputHandler.keys[GLFW_KEY_I]) {
      env.inputHandler.keys[GLFW_KEY_I] = false;
      debugMode = !debugMode;
    }
    if (env.inputHandler.keys[GLFW_KEY_LEFT_BRACKET]) {
      env.inputHandler.keys[GLFW_KEY_LEFT_BRACKET] = false;
      chunkManager.decreaseRenderDistance();
    }
    if (env.inputHandler.keys[GLFW_KEY_RIGHT_BRACKET]) {
      env.inputHandler.keys[GLFW_KEY_RIGHT_BRACKET] = false;
      chunkManager.increaseRenderDistance();
    }
  }
}
