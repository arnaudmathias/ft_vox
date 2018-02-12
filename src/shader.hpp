#pragma once
#include <sys/stat.h>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <string>
#include "env.hpp"
#include "renderer.hpp"

enum class ShaderType { NORMAL };

struct ShaderFile {
  std::string filename;
  std::time_t last_modification;
};

class Shader {
 public:
  Shader(ShaderType type, std::string vertFilename, std::string fragFilename);
  Shader(Shader const &src);
  ~Shader(void);
  Shader &operator=(Shader const &rhs);

  GLuint id;
  ShaderType type;
  void use() const;
  void reload();

 private:
  Shader(void);
  GLuint loadVertex(std::string filename);
  GLuint loadFragment(std::string filename);
  GLuint compileShader(const std::string source, GLuint shaderType);
  GLuint linkShaders(GLuint vertexID, GLuint fragID);
  const std::string getShaderSource(std::string filename);
  ShaderFile _vertex;
  ShaderFile _fragment;
};

void printShaderError(GLuint shade, std::string filename);
void printLinkError(GLuint program, std::string vextexFilename,
                    std::string fragmentFilename);
std::time_t getLastModificationTime(std::string filename);
