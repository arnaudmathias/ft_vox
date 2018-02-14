#include "renderer.hpp"

std::vector<glm::vec3> skyboxVertices = {
    {-1.0f, 1.0f, -1.0f},  {-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f},
    {1.0f, -1.0f, -1.0f},  {1.0f, 1.0f, -1.0f},   {-1.0f, 1.0f, -1.0f},
    {-1.0f, -1.0f, 1.0f},  {-1.0f, -1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f},
    {-1.0f, 1.0f, -1.0f},  {-1.0f, 1.0f, 1.0f},   {-1.0f, -1.0f, 1.0f},
    {1.0f, -1.0f, -1.0f},  {1.0f, -1.0f, 1.0f},   {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},    {1.0f, 1.0f, -1.0f},   {1.0f, -1.0f, -1.0f},
    {-1.0f, -1.0f, 1.0f},  {-1.0f, 1.0f, 1.0f},   {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},    {1.0f, -1.0f, 1.0f},   {-1.0f, -1.0f, 1.0f},
    {-1.0f, 1.0f, -1.0f},  {1.0f, 1.0f, -1.0f},   {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},    {-1.0f, 1.0f, 1.0f},   {-1.0f, 1.0f, -1.0f},
    {-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, 1.0f},  {1.0f, -1.0f, -1.0f},
    {1.0f, -1.0f, -1.0f},  {-1.0f, -1.0f, 1.0f},  {1.0f, -1.0f, 1.0f}};

Renderer::Renderer(void) {
  _shader =
      new Shader(ShaderType::NORMAL, "shaders/vox.vert", "shaders/vox.frag");
}

Renderer::Renderer(int width, int height)
    : _width(width),
      _height(height),
      _cubeMapVao(nullptr),
      _cubeMapShader(nullptr) {
  _shader =
      new Shader(ShaderType::NORMAL, "shaders/vox.vert", "shaders/vox.frag");
}

Renderer::Renderer(Renderer const &src) { *this = src; }

Renderer::~Renderer(void) {
  delete _shader;
  delete _cubeMapVao;
  delete _cubeMapShader;
}

Renderer &Renderer::operator=(Renderer const &rhs) {
  if (this != &rhs) {
    this->_renderAttribs = rhs._renderAttribs;
  }
  return (*this);
}

void Renderer::addRenderAttrib(const RenderAttrib &renderAttrib) {
  this->_renderAttribs.push_back(renderAttrib);
}

void Renderer::bindTexture(Texture *texture, int &texture_binded,
                           GLenum tex_slot) {
  if (texture != nullptr) {
    if (texture->id != texture_binded && texture->id > 0) {
      glActiveTexture(tex_slot);
      glBindTexture(GL_TEXTURE_2D, texture->id);
      texture_binded = texture->id;
    }
  } else {
    glActiveTexture(tex_slot);
    glBindTexture(GL_TEXTURE_2D, 0);
    texture_binded = -1;
  }
}

void Renderer::switchTextures(const std::array<Texture *, 4> &textures,
                              std::array<int, 4> &tex_channel) {
  bindTexture(textures[0], tex_channel[0], GL_TEXTURE0);
  bindTexture(textures[1], tex_channel[1], GL_TEXTURE1);
  bindTexture(textures[2], tex_channel[2], GL_TEXTURE2);
  bindTexture(textures[3], tex_channel[3], GL_TEXTURE3);
}

void Renderer::switchShader(GLuint shader_id, int &current_shader_id) {
  if (shader_id > 0 && shader_id != current_shader_id) {
    glUseProgram(shader_id);
    setUniform(glGetUniformLocation(shader_id, "iResolution"),
               this->uniforms.iResolution);
    setUniform(glGetUniformLocation(shader_id, "iTime"), this->uniforms.iTime);
    setUniform(glGetUniformLocation(shader_id, "iTimeDelta"),
               this->uniforms.iTimeDelta);
    setUniform(glGetUniformLocation(shader_id, "iFrame"),
               this->uniforms.iFrame);
    setUniform(glGetUniformLocation(shader_id, "iSampleRate"),
               this->uniforms.iSampleRate);
    setUniform(glGetUniformLocation(shader_id, "camPos"),
               this->uniforms.camPos);
    setUniform(glGetUniformLocation(shader_id, "camDir"),
               this->uniforms.camDir);
    setUniform(glGetUniformLocation(shader_id, "camRight"),
               this->uniforms.camRight);
    setUniform(glGetUniformLocation(shader_id, "camUp"), this->uniforms.camUp);
    setUniform(glGetUniformLocation(shader_id, "fovYscale"),
               this->uniforms.fovYscale);
    setUniform(glGetUniformLocation(shader_id, "zNear"), this->uniforms.zNear);
    setUniform(glGetUniformLocation(shader_id, "zFar"), this->uniforms.zFar);
    current_shader_id = shader_id;
  }
}

void Renderer::updateUniforms(const RenderAttrib &attrib, const int shader_id,
                              std::array<int, 4> &tex_channel) {
  if (shader_id > 0 && attrib.vaos.size() > 0) {
    setUniform(glGetUniformLocation(shader_id, "iChannel0"), 0);
    setUniform(glGetUniformLocation(shader_id, "iChannel1"), 1);
    setUniform(glGetUniformLocation(shader_id, "iChannel2"), 2);
    setUniform(glGetUniformLocation(shader_id, "iChannel3"), 3);
    glm::mat4 mvp = this->uniforms.proj * this->uniforms.view * attrib.model;
    setUniform(glGetUniformLocation(shader_id, "MVP"), mvp);
    setUniform(glGetUniformLocation(shader_id, "P"), this->uniforms.proj);
    setUniform(glGetUniformLocation(shader_id, "V"), this->uniforms.view);
    setUniform(glGetUniformLocation(shader_id, "M"), attrib.model);
  }
}

void Renderer::draw() {
  std::sort(_renderAttribs.begin(), _renderAttribs.end());
  int shader_id = -1;
  std::array<int, 4> tex_channel = {-1, -1, -1, -1};
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  for (const auto &attrib : this->_renderAttribs) {
    switchShader(_shader->id, shader_id);
    updateUniforms(attrib, _shader->id, tex_channel);
    switchTextures({attrib.iChannel0, attrib.iChannel1, attrib.iChannel2,
                    attrib.iChannel3},
                   tex_channel);
    for (const auto &vao : attrib.vaos) {
      glBindVertexArray(vao->vao);
      glDrawArrays(GL_TRIANGLES, 0, vao->vertices_size);
    }
  }
  glBindVertexArray(0);
}

void Renderer::flush() { this->_renderAttribs.clear(); }

void Renderer::reset() { this->_renderAttribs.clear(); }

int Renderer::getScreenWidth() { return (this->_width); }

int Renderer::getScreenHeight() { return (this->_height); }

void Renderer::loadCubeMap(std::string vertex_sha, std::string fragment_sha,
                           const std::vector<std::string> &textures) {
  this->_cubeMapTexture = new Texture(textures);
  this->_cubeMapShader =
      new Shader(ShaderType::NORMAL, vertex_sha, fragment_sha);
  this->_cubeMapVao = new VAO(skyboxVertices);
}

bool RenderAttrib::operator<(const struct RenderAttrib &rhs) const {
  return (this->vaos[0]->vao < rhs.vaos[0]->vao);
}
