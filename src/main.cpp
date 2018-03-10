#include <iomanip>
#include <list>
#include "env.hpp"
#include "game.hpp"
#include "renderer.hpp"

int main(int argc, char **argv) {
  uint32_t seed = 42;
  if (argc > 2 || strcmp(argv[0], "./ft_vox") != 0) {
    std::cout << "Usage: ./ft_vox [seed]" << std::endl;
    return (EXIT_FAILURE);
  }
  if (argc == 2) {
    try {
      seed = std::stoi(argv[1]);
    } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
    }
  }
  Env env(1280, 720);
  Renderer renderer(env.width, env.height);
  renderer.loadCubeMap(
      "shaders/skybox.vert", "shaders/skybox.frag",
      {"textures/skybox_side.png", "textures/skybox_side.png",
       "textures/skybox_up.png", "textures/skybox_bottom.png",
       "textures/skybox_side.png", "textures/skybox_side.png"});
  Game game(seed);
  bool wireframe = false;
  while (!glfwWindowShouldClose(env.window)) {
    env.update();
    glfwPollEvents();
    game.update(env);
    renderer.update(env);
    renderer.clearScreen();
    game.render(env, renderer);
    glfwSwapBuffers(env.window);
    // GL_DUMP_ERROR("draw loop");
    if (env.inputHandler.keys[GLFW_KEY_ESCAPE]) {
      glfwSetWindowShouldClose(env.window, 1);
    }
    if (env.inputHandler.keys[GLFW_KEY_M]) {
      env.inputHandler.keys[GLFW_KEY_M] = false;
      wireframe = !wireframe;
      renderer.switchPolygonMode(wireframe ? PolygonMode::Line
                                           : PolygonMode::Fill);
    }
    if (env.inputHandler.keys[GLFW_KEY_F]) {
      env.inputHandler.keys[GLFW_KEY_F] = false;
      // env.toggleFullscreen();
    }
  }
  return (EXIT_SUCCESS);
}
