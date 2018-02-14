#pragma once
#include <algorithm>
#include <map>
#include <vector>
#include "env.hpp"
#include "ft_vox.hpp"
#include "shader.hpp"

class Shader;

struct Texture {
  Texture(int width, int height);
  Texture(std::string filename);
  ~Texture();
  std::string filename;
  GLuint id;
  int height;
  int width;
};

struct Uniforms {
  glm::mat4 view;
  glm::mat4 proj;
  GLint location;
  glm::vec3 iResolution;
  float iTime;
  float iTimeDelta;
  float iFrame;
  float iChannelTime[4];
  glm::vec4 iMouse;
  glm::vec4 iDate;
  float iSampleRate;
  glm::vec3 iChannelResolution[4];
  glm::vec3 camPos;
  glm::vec3 camDir;
  glm::vec3 camUp;
  glm::vec3 camRight;
  float zFar;
  float zNear;
  float fovYscale;
  /* samplerXX iChanneli; */
};

struct VAO {
  VAO(const std::vector<Vertex>& vertices);
  VAO(const std::vector<glm::vec3>& positions);
  ~VAO();
  GLuint vao;
  GLsizei vertices_size;
  GLsizei indices_size;
  std::vector<Vertex> vertices;

 private:
  GLuint _vbo;
};

struct RenderAttrib {
  VAO* vao;
  glm::mat4 model;
  Texture* iChannel0;
  Texture* iChannel1;
  Texture* iChannel2;
  Texture* iChannel3;
  RenderAttrib()
      : vao(nullptr),
        iChannel0(nullptr),
        iChannel1(nullptr),
        iChannel2(nullptr),
        iChannel3(nullptr){};

  bool operator<(const struct RenderAttrib& rhs) const;
};

class Renderer {
 public:
  Renderer(int width, int height);
  Renderer(Renderer const& src);
  virtual ~Renderer(void);
  Renderer& operator=(Renderer const& rhs);
  void addRenderAttrib(const RenderAttrib& renderAttrib);
  void draw();
  void flush();
  void reset();
  int getScreenWidth();
  int getScreenHeight();
  void loadCubeMap(std::string vertex_sha, std::string fragment_sha,
                   const std::vector<std::string>& textures);
  Uniforms uniforms;

 private:
  int _width;
  int _height;
  GLuint _cubeMap;
  VAO* _cubeMapVao;
  Shader* _cubeMapShader;
  Shader* _shader;
  Renderer(void);
  std::vector<RenderAttrib> _renderAttribs;
  void bindTexture(Texture* texture, int& texture_binded, GLenum tex_slot);
  void switchTextures(const std::array<Texture*, 4>& textures,
                      std::array<int, 4>& tex_channel);
  void switchShader(GLuint shader_id, int& current_shader_id);
  void updateUniforms(const RenderAttrib& attrib, const int shader_id,
                      std::array<int, 4>& tex_channel);
};

static inline void setUniform(const GLint& location, const float& data) {
  glUniform1f(location, data);
}
static inline void setUniform(const GLint& location, const int& data) {
  glUniform1i(location, data);
}
static inline void setUniform(const GLint& location, const glm::vec2& data) {
  glUniform2fv(location, 1, static_cast<const GLfloat*>(glm::value_ptr(data)));
}
static inline void setUniform(const GLint& location, const glm::vec3& data) {
  glUniform3fv(location, 1, static_cast<const GLfloat*>(glm::value_ptr(data)));
}
static inline void setUniform(const GLint& location, const glm::vec4& data) {
  glUniform4fv(location, 1, static_cast<const GLfloat*>(glm::value_ptr(data)));
}
static inline void setUniform(const GLint& location, const glm::mat4& data) {
  glUniformMatrix4fv(location, 1, GL_FALSE,
                     static_cast<const GLfloat*>(glm::value_ptr(data)));
}
