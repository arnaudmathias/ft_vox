#include "renderer.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

Renderer::Renderer(int width, int height) : _width(width), _height(height) {
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

void Renderer::switchTextures(std::array<Texture *, 4> textures,
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
  if (shader_id > 0 && attrib.vao != nullptr) {
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
  // printRenderAttribs();
  int shader_id = -1;
  std::array<int, 4> tex_channel = {-1, -1, -1, -1};
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  for (const auto &attrib : this->_renderAttribs) {
    switchShader(_shader->id, shader_id);
    updateUniforms(attrib, _shader->id, tex_channel);
    switchTextures({attrib.iChannel0, attrib.iChannel1, attrib.iChannel2,
                    attrib.iChannel3},
                   tex_channel);
    glBindVertexArray(attrib.vao->vao);
    glDrawArrays(GL_TRIANGLES, 0, attrib.vao->vertices_size);
  }
  glBindVertexArray(0);
}

void Renderer::flush() { this->_renderAttribs.clear(); }

void Renderer::printRenderAttribs() {
  std::cout << "------------" << std::endl;
  for (const auto &attrib : this->_renderAttribs) {
    // std::cout << "shader: " << attrib.shader;
    if (attrib.vao != nullptr) {
      std::cout << " | vao:" << attrib.vao->vao;
    }
    if (attrib.iChannel0 != nullptr) {
      std::cout << " | texture:" << attrib.iChannel0->id;
    }
    std::cout << std::endl;
  }
  std::cout << "------------" << std::endl;
}

void Renderer::reset() {
  /* this->view = glm::mat4(); */
  /* this->proj = glm::mat4(); */
  this->_renderAttribs.clear();
}

int Renderer::getScreenWidth() { return (this->_width); }

int Renderer::getScreenHeight() { return (this->_height); }

void Renderer::loadCubeMap(std::string vertex_sha, std::string fragment_sha,
                           std::vector<std::string> textures) {
  glGenTextures(1, &this->_cubeMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, this->_cubeMap);
  // stbi_set_flip_vertically_on_load(true);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < textures.size(); i++) {
    unsigned char *data =
        stbi_load(textures[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                   0, GL_RGB, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      std::cout << "Cubemap texture failed to load: " << textures[i]
                << std::endl;
      stbi_image_free(data);
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  this->_cubeMapShader =
      new Shader(ShaderType::NORMAL, vertex_sha, fragment_sha);
  this->_cubeMapVao = new VAO(skyboxVertices);
}

bool RenderAttrib::operator<(const struct RenderAttrib &rhs) const {
  // sort renderAttrib by shader and vao
  // Must avoid as much as possible context switch
  /*if (this->shader->id == rhs.shader->id && this->vao != nullptr &&
      rhs.vao != nullptr) { */
  return (this->vao->vao < rhs.vao->vao);
  //}
  // return (this->shader->id < rhs.shader->id);
}

VAO::VAO(std::vector<Vertex> vertices) : vertices(vertices) {
  this->_vbo = 0;
  this->vao = 0;
  this->vertices_size = vertices.size();
  this->indices_size = 0;
  glGenBuffers(1, &this->_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBufferData(GL_ARRAY_BUFFER, this->vertices_size * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid *)offsetof(Vertex, position));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
}

VAO::VAO(std::vector<glm::vec3> positions) {
  this->_vbo = 0;
  this->vao = 0;
  this->vertices_size = positions.size();
  this->indices_size = 0;

  glGenBuffers(1, &this->_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBufferData(GL_ARRAY_BUFFER, this->vertices_size * sizeof(glm::vec3),
               positions.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &this->vao);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        (GLvoid *)0);

  glEnableVertexAttribArray(0);
}

VAO::~VAO() {
  if (this->_vbo != 0) glDeleteBuffers(1, &this->_vbo);
  if (this->vao != 0) glDeleteVertexArrays(1, &this->vao);
}

Texture::Texture(int width, int height)
    : id(0), filename("renderbuffer"), width(width), height(height) {
  glGenTextures(1, &this->id);
  glBindTexture(GL_TEXTURE_2D, this->id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);

  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(std::string filename) : id(0), filename(filename) {
  int texChannels;
  stbi_set_flip_vertically_on_load(true);
  stbi_uc *pixels = stbi_load(filename.c_str(), &this->width, &this->height,
                              &texChannels, STBI_rgb_alpha);
  if (pixels != nullptr) {
    glGenTextures(1, &this->id);

    glBindTexture(GL_TEXTURE_2D, this->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
  } else {
    throw std::runtime_error("Invalid texture file");
  }
}

Texture::~Texture() {
  if (this->id != 0) {
    glDeleteTextures(1, &this->id);
  }
}
