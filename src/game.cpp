#include "game.hpp"

Game::Game(void) : Game(42) {}

Game::Game(uint32_t seed) : _chunkManager(seed) {
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
    struct HitInfo rm_cube =
	_chunkManager.rayCast(_camera->dir, _camera->pos, 5.0f);
    _chunkManager.set_block(Block(Material::Air), rm_cube.pos);
  }
  if (env.inputHandler.mouse_keys[GLFW_MOUSE_BUTTON_RIGHT]) {
    env.inputHandler.mouse_keys[GLFW_MOUSE_BUTTON_RIGHT] = false;
    struct HitInfo add_cube =
	_chunkManager.rayCast(_camera->dir, _camera->pos, 5.0f);
    if (add_cube.hit) {
      _chunkManager.add_block(add_cube.pos +
			      glm::ivec3(mesher::get_normal(add_cube.side)));
    }
  }
  if (env.inputHandler.keys[GLFW_KEY_KP_0]) {
    env.inputHandler.keys[GLFW_KEY_KP_0] = false;
    _chunkManager.setBlockType(Block(Material::Dirt));
  }
  if (env.inputHandler.keys[GLFW_KEY_KP_1]) {
    env.inputHandler.keys[GLFW_KEY_KP_1] = false;
    _chunkManager.setBlockType(Block(Material::Bedrock));
  }
  if (env.inputHandler.keys[GLFW_KEY_KP_2]) {
    env.inputHandler.keys[GLFW_KEY_KP_2] = false;
    _chunkManager.setBlockType(Block(Material::Sand));
  }
  if (env.inputHandler.keys[GLFW_KEY_T]) {
    env.inputHandler.keys[GLFW_KEY_T] = false;
    struct HitInfo add_cube =
	_chunkManager.rayCast(_camera->dir, _camera->pos, 50.0f);
    if (add_cube.hit) {
      _chunkManager.point_exploding(add_cube.pos, 10.f);
    }
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
	if (env.inputHandler.keys[GLFW_KEY_P]) {
    env.inputHandler.keys[GLFW_KEY_P] = false;
    _chunkManager.Draw_earth();
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
  float fwidth = static_cast<float>(renderer.getScreenWidth());
  float fheight = static_cast<float>(renderer.getScreenHeight());
  renderer.uniforms.view = _camera->view;
  renderer.uniforms.proj = _camera->proj;
  renderer.uniforms.view_proj = _camera->proj * _camera->view;
  _chunkManager.setRenderAttributes(renderer, _camera->pos);
  renderer.draw();
  renderer.flush();
  if (_debugMode) {
    print_debug_info(env, renderer, *_camera);
    _chunkManager.print_chunkmanager_info(renderer, fheight, fwidth);
  }
  renderer.renderUI("textures/cursor.png", fwidth / 2.0f, fheight / 2.0f, 0.4f,
		    true);
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
