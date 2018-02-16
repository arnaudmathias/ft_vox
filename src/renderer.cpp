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

Renderer::Renderer(void) : Renderer(0, 0) {}

Renderer::Renderer(int width, int height)
    : _width(width),
      _height(height),
      _textureAtlas(nullptr),
      _cubeMapTexture(nullptr),
      _cubeMapVao(nullptr),
      _cubeMapShader(nullptr) {
  _shader =
      new Shader(ShaderType::NORMAL, "shaders/vox.vert", "shaders/vox.frag");
  _textureAtlas = new Texture("textures/terrain.png", 16, 16);
  std::cout << "texture id: " << _textureAtlas->id << std::endl;
}

Renderer::Renderer(Renderer const &src) { *this = src; }

Renderer::~Renderer(void) {
  delete _textureAtlas;
  delete _shader;
  delete _cubeMapVao;
  delete _cubeMapShader;
  delete _cubeMapTexture;
}

Renderer &Renderer::operator=(Renderer const &rhs) {
  if (this != &rhs) {
    this->_renderAttribs = rhs._renderAttribs;
  }
  return (*this);
}

void Renderer::renderText(float pos_x, float pos_y, float scale,
                          std::string text, glm::vec3 color) {
  glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->_width),
                                    0.0f, static_cast<float>(this->_height));
  textRenderer.renderText(pos_x, pos_y, scale, text, color, projection);
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
    setUniform(glGetUniformLocation(shader_id, "texture_array"), 0);
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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, _textureAtlas->id);
    /*switchTextures({attrib.iChannel0, attrib.iChannel1, attrib.iChannel2,
                    attrib.iChannel3},
                   tex_channel);*/
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

TextRenderer::TextRenderer(void) {
  Shader shader(ShaderType::NORMAL, "shaders/text.vert", "shaders/text.frag");
  this->_shader_id = shader.id;

  FT_Library ft;
  if (FT_Init_FreeType(&ft))
    std::cout << "Could not init FreeType Library" << std::endl;

  FT_Face face;
  if (FT_New_Face(ft, "fonts/minecraft.ttf", 0, &face))
    std::cout << "Failed to load font" << std::endl;

  FT_Set_Pixel_Sizes(face, 0, 48);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  for (GLubyte c = 0; c < 128; c++) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cout << "Failed to load Glyph" << std::endl;
      continue;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                 face->glyph->bitmap.buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character character = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        static_cast<GLuint>(face->glyph->advance.x)};
    this->_characters.insert(std::pair<GLchar, Character>(c, character));
  }
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  glGenVertexArrays(1, &this->_vao);
  glGenBuffers(1, &this->_vbo);

  glBindVertexArray(this->_vao);
  glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

TextRenderer::~TextRenderer() {
  for (auto &character : this->_characters) {
    glDeleteTextures(1, &character.second.textureID);
  }
}

void TextRenderer::renderText(float pos_x, float pos_y, float scale,
                              std::string text, glm::vec3 color,
                              glm::mat4 ortho) {
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(_shader_id);
  glUniformMatrix4fv(glGetUniformLocation(_shader_id, "proj"), 1, GL_FALSE,
                     glm::value_ptr(ortho));
  glUniform3fv(glGetUniformLocation(_shader_id, "text_color"), 1,
               glm::value_ptr(color));
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(this->_vao);
  for (char &c : text) {
    if (static_cast<int>(c) > 0 && static_cast<int>(c) < 128) {
      Character ch = this->_characters[static_cast<int>(c)];
      GLfloat xpos = pos_x + ch.bearing.x * scale;
      GLfloat ypos = pos_y - (ch.size.y - ch.bearing.y) * scale;

      GLfloat w = ch.size.x * scale;
      GLfloat h = ch.size.y * scale;
      GLfloat vertices[6][4] = {
          {xpos, ypos + h, 0.0, 0.0},    {xpos, ypos, 0.0, 1.0},
          {xpos + w, ypos, 1.0, 1.0},

          {xpos, ypos + h, 0.0, 0.0},    {xpos + w, ypos, 1.0, 1.0},
          {xpos + w, ypos + h, 1.0, 0.0}};
      glBindTexture(GL_TEXTURE_2D, ch.textureID);
      glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      pos_x += (ch.advanceOffset >> 6) * scale;
    }
  }
  glBindVertexArray(0);
  glEnable(GL_DEPTH_TEST);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_BLEND);
}
