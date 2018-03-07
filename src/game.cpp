#include "game.hpp"

Game::Game(void) {
  _camera =
      new Camera(glm::vec3(0.0f, 125.0f, 1.0f), glm::vec3(0.0f, 125.0f, 0.0f));
}

Game::Game(Game const& src) { *this = src; }

Game::~Game(void) { delete _camera; }

Game& Game::operator=(Game const& rhs) {
  if (this != &rhs) {
    this->_chunkManager = rhs._chunkManager;
    this->_debugMode = rhs._debugMode;
    this->_camera = new Camera(*rhs._camera);
  }
  return (*this);
}
void Game::update(Env& env) {
  _camera->update(env);
  _chunkManager.update(_camera->pos);
  if (env.inputHandler.mouse_keys[GLFW_MOUSE_BUTTON_LEFT]) {
    env.inputHandler.mouse_keys[GLFW_MOUSE_BUTTON_LEFT] = false;
    _chunkManager.rayCast(_camera->dir, _camera->pos, 5.0f);
  }
  if (env.inputHandler.keys[GLFW_KEY_C]) {
    env.inputHandler.keys[GLFW_KEY_C] = false;
    _chunkManager.setMeshingType(MeshingType::Culling);
  }
  if (env.inputHandler.keys[GLFW_KEY_G]) {
    env.inputHandler.keys[GLFW_KEY_G] = false;
    _chunkManager.setMeshingType(MeshingType::Greedy);
  }
  if (env.inputHandler.keys[GLFW_KEY_I]) {
    env.inputHandler.keys[GLFW_KEY_I] = false;
    _debugMode = !_debugMode;
  }
  if (env.inputHandler.keys[GLFW_KEY_LEFT_BRACKET]) {
    env.inputHandler.keys[GLFW_KEY_LEFT_BRACKET] = false;
    _chunkManager.decreaseRenderDistance();
  }
  if (env.inputHandler.keys[GLFW_KEY_RIGHT_BRACKET]) {
    env.inputHandler.keys[GLFW_KEY_RIGHT_BRACKET] = false;
    _chunkManager.increaseRenderDistance();
  }
}

void Game::render(const Env& env, Renderer& renderer) {
  renderer.uniforms.view = _camera->view;
  renderer.uniforms.proj = _camera->proj;
  renderer.uniforms.view_proj = _camera->proj * _camera->view;
  _chunkManager.setRenderAttributes(renderer, _camera->pos);
  renderer.draw();
  renderer.flush();
  if (_debugMode) {
    print_debug_info(env, renderer, *_camera);
    _chunkManager.print_chunkmanager_info(
        renderer, static_cast<float>(renderer.getScreenHeight()),
        static_cast<float>(renderer.getScreenWidth()));
  }
}

std::string float_to_string(float f, int prec) {
  std::ostringstream out;
  out << std::setprecision(prec) << std::fixed << f;
  return out.str();
}

void Game::print_debug_info(const Env& env, Renderer& renderer,
                            Camera& camera) {
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
}