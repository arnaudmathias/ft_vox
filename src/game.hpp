#pragma once
#include <iomanip>
#include "camera.hpp"
#include "chunk.hpp"
#include "ft_vox.hpp"
#include "renderer.hpp"
class Game {
 public:
  Game(uint32_t);
  Game(Game const& src);
  virtual ~Game(void);
  Game& operator=(Game const& rhs);
  void update(Env& env);
  void render(const Env& env, Renderer& renderer);

 private:
  Game(void);
  ChunkManager _chunkManager;
  bool _debugMode;
  Camera* _camera;
  RenderAttrib faceRenderAttrib;
  struct HitInfo _last_hit;
  void print_debug_info(const Env& env, Renderer& renderer, Camera& camera);
};
