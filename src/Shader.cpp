//
// Created by Hugo Zheng on 2021-07-11.
//

#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  // 1. retrieve source code
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vertexFile;
  std::ifstream fragmentFile;

  // ensure ifstream objects can throw exceptions
  vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    // open files
    vertexFile.open(vertexPath);
    fragmentFile.open(fragmentPath);
    std::stringstream vertexStream, fragmentStream;

    // read file
    vertexStream << vertexFile.rdbuf();
    fragmentStream << fragmentFile.rdbuf();

    // close file handlers
    vertexFile.close();
    fragmentFile.close();

    vertexCode = vertexStream.str();
    fragmentCode = fragmentStream.str();
  } catch (std::ifstream::failure &e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }

  // 2. compile shaders
  unsigned int vertex, fragment;
  int success;

  vertex = glCreateShader(GL_VERTEX_SHADER);
  // convert string to c-string: separate steps to avoid unexpected behavior
  const char *vertexSource = vertexCode.c_str();
  glShaderSource(vertex, 1, &vertexSource, nullptr);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    int length;
    glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &length);
    char *infoLog = new char[length];
    glGetShaderInfoLog(vertex, length, &length, infoLog);
    std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    delete[] infoLog;
  }

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  // convert string to c-string: separate steps to avoid unexpected behavior
  const char *fragmentSource = fragmentCode.c_str();
  glShaderSource(fragment, 1, &fragmentSource, nullptr);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    int length;
    glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length);
    char *infoLog = new char[length];
    glGetShaderInfoLog(fragment, length, &length, infoLog);
    std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    delete[] infoLog;
  }

  // create and link shader program
  id = glCreateProgram();
  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  glLinkProgram(id);
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    int length;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
    char *infoLog = new char[length];
    glGetProgramInfoLog(id, length, &length, infoLog);
    std::cerr << "ERROR:SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    delete[] infoLog;
  }

  // delete the shaders since they are already linked
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::use() const {
  glUseProgram(id);
}

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(id, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, glm::vec3 value) const {
  glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const {
  glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}




