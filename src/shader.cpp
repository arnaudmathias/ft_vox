#include "shader.hpp"

Shader::Shader(void) {}

Shader::Shader(Shader const &src) { *this = src; }

Shader::~Shader(void) {}

Shader &Shader::operator=(Shader const &rhs) {
  if (this != &rhs) {
    this->id = rhs.id;
    this->type = rhs.type;
    this->_vertex = rhs._vertex;
    this->_fragment = rhs._fragment;
  }
  return (*this);
}

Shader::Shader(ShaderType type, std::string vertFilename,
               std::string fragmentFilename)
    : id(-1), type(type) {
  GLuint vertexID;
  GLuint fragID;
  GLint err = -1;

  this->_vertex.filename = vertFilename;
  this->_vertex.last_modification = getLastModificationTime(vertFilename);
  this->_fragment.filename = fragmentFilename;
  this->_fragment.last_modification = getLastModificationTime(fragmentFilename);
  vertexID = loadVertex(vertFilename);
  fragID = loadFragment(fragmentFilename);
  id = linkShaders(vertexID, fragID);
  glUseProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &err);
  if (GL_TRUE != err) printLinkError(id, vertFilename, fragmentFilename);
}

GLuint Shader::loadVertex(std::string filename) {
  GLuint vertexID;
  vertexID = compileShader(getShaderSource(filename), GL_VERTEX_SHADER);
  return (vertexID);
}

GLuint Shader::loadFragment(std::string filename) {
  GLuint fragID;
  fragID = compileShader(getShaderSource(filename), GL_FRAGMENT_SHADER);
  return (fragID);
}

GLuint Shader::linkShaders(GLuint vertexID, GLuint fragID) {
  GLuint id = glCreateProgram();

  glAttachShader(id, vertexID);
  glAttachShader(id, fragID);
  glLinkProgram(id);
  glDeleteShader(vertexID);
  glDeleteShader(fragID);
  return (id);
}

const std::string Shader::getShaderSource(std::string filename) {
  std::string line;
  std::string fileContent = "";
  int err = -1;

  std::fstream shaderFile(filename);
  if (shaderFile) {
    while (getline(shaderFile, line)) fileContent += line + "\n";
    shaderFile.close();
  } else
    std::cout << "Invalid shader file name: " << filename << "\n";
  return (fileContent);
}

GLuint Shader::compileShader(std::string source, GLuint shaderType) {
  GLuint id;
  int err = -1;

  id = glCreateShader(shaderType);
  const char *fileContentChar = source.c_str();
  glShaderSource(id, 1, &fileContentChar, NULL);
  glCompileShader(id);
  glGetShaderiv(id, GL_COMPILE_STATUS, &err);
  if (GL_TRUE != err) printShaderError(id, "");
  return (id);
}

void Shader::use() const { glUseProgram(this->id); }

void Shader::reload() {
  GLuint vertexID;
  GLuint fragID;
  GLint err = -1;
  bool need_reload = false;
  std::time_t vertex_last_mod = getLastModificationTime(this->_vertex.filename);
  std::time_t frag_last_mod = getLastModificationTime(this->_fragment.filename);

  if ((this->_vertex.last_modification != -1 &&
       this->_vertex.last_modification < vertex_last_mod) ||
      (this->_fragment.last_modification != -1 &&
       this->_fragment.last_modification < frag_last_mod)) {
    this->_vertex.last_modification = vertex_last_mod;
    this->_fragment.last_modification = frag_last_mod;
    vertexID = loadVertex(this->_vertex.filename);
    fragID = loadFragment(this->_fragment.filename);
    id = linkShaders(vertexID, fragID);
    glUseProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &err);
    if (GL_TRUE != err)
      printLinkError(id, this->_vertex.filename, this->_fragment.filename);
  }
}

void printLinkError(GLuint program, std::string vfn, std::string ffn) {
  char log[2048];
  int max_length;
  int index;

  max_length = 2048;
  index = 0;
  glGetProgramInfoLog(program, max_length, &index, log);
  glGetShaderInfoLog(program, max_length, &index, log);
  std::cout << "Cannot link shaders: " << vfn << " and " << ffn << "\n" << log;
  // exit(0);
}

void printShaderError(GLuint shader, std::string file_name) {
  char log[2048];
  int max_length;
  int index;

  max_length = 2048;
  index = 0;
  glGetShaderInfoLog(shader, max_length, &index, log);
  std::cout << "Cannot compile shader: " << file_name << "\n" << log;
  // exit(0);
}

std::time_t getLastModificationTime(std::string filename) {
  struct stat result;
  if (!filename.empty()) {
    if (stat(filename.c_str(), &result) == 0) {
      std::time_t mod_time = result.st_mtime;
      return (mod_time);
    }
  }
  return ((std::time_t)-1);
}
