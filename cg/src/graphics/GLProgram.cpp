//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2018 Orthrus Group.                         |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: GLProgram.cpp
// ========
// Source file for GLSL program.
//
// Author: Paulo Pagliosa
// Last revision: 10/08/2018

#include "graphics/GLProgram.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace cg
{ // begin namespace cg

namespace GLSL
{ // begin namespace GLSL

enum ErrorCode
{
  UNABLE_TO_OPEN_SHADER_FILE,
  COMPILE_ERROR,
  CANNOT_ATTACH_SHADER,
  LINK_ERROR,
  CANNOT_USE_PROGRAM,
  PROGRAM_NOT_IN_USE,
  VARIABLE_NOT_FOUND,
  SUBROUTINE_NOT_FOUND
};

static const char* errorMessages[] =
{
  "Unable to open shader file '%s'",
  "'%s': compile error:\n%s",
  "'%s': cannot attach shader '%s': program in use",
  "'%s': link error:\n%s",
  "'%s': cannot use program: no shader",
  "'%s': program not in use",
  "'%s': variable '%s' not found",
  "'%s': subroutine '%s' not found"
};

static void
error(ErrorCode code, ...)
{
  const int bufferSize{4096};
  char buffer[bufferSize];
  va_list args;

  va_start(args, code);
  std::vsnprintf(buffer, bufferSize, errorMessages[code], args);
  throw std::runtime_error(buffer);
}

static char*
readShaderFile(const char* fileName)
{
  using namespace std;

  auto file = ifstream(fileName, ios::in | ios::binary | ios::ate);

  if (!file.is_open())
    error(UNABLE_TO_OPEN_SHADER_FILE, fileName);

  auto size = file.tellg();
  auto buffer = new char[size + streamoff{1}];

  file.seekg(0, ios::beg);
  file.read(buffer, size);
  buffer[size] = 0;
  file.close();
  return buffer;
}

using ObjectParamFunc = std::function<void(GLuint, GLenum, GLint*)>;
using InfoLogFunc = std::function<void(GLuint, GLsizei, GLsizei*, GLchar*)>;

static std::string
infoLog(GLuint obj, ObjectParamFunc getParam, InfoLogFunc getLog)
{
  GLint maxLen{0};
  std::string log;

  getParam(obj, GL_INFO_LOG_LENGTH, &maxLen);
  if (maxLen > 0)
  {
    auto buf = new char[maxLen];
    GLsizei len{0};

    getLog(obj, maxLen, &len, buf);
    log = std::string(buf, len);
    delete[]buf;
  }
  return log;
}


/////////////////////////////////////////////////////////////////////
//
// Shader: GLSL shader class
// ======
class Shader
{
public:
  // Constructor.
  Shader(GLenum shaderType):
    _handle{glCreateShader(shaderType)},
    _name{shaderName(shaderType)}
  {
    // do nothing
  }

  // Destructor.
  ~Shader()
  {
    glDeleteShader(_handle);
  }

  // Loads source from file.
  void loadSourceFromFile(const char*);

  // Sets source.
  void setSource(const char*);

  // Casts to handle type.
  operator GLuint() const
  {
    return _handle;
  }

  // Returns the name of this shader.
  auto name() const
  {
    return _name.c_str();
  }

  // Returns true if this shader is compiled.
  bool isCompiled() const
  {
    return _compiled;
  }

private:
  GLuint _handle;
  std::string _name;
  bool _compiled{};

  // Compiles this shader.
  void compile();

  // Returns the shader name.
  static const char* shaderName(GLenum shaderType)
  {
    switch (shaderType)
    {
      default:
        return "unknown shader";
      case GL_VERTEX_SHADER:
        return "vertex shader";
      case GL_TESS_CONTROL_SHADER:
        return "tess control shader";
      case GL_TESS_EVALUATION_SHADER:
        return "tess evaluation shader";
      case GL_GEOMETRY_SHADER:
        return "geometry shader";
      case GL_FRAGMENT_SHADER:
        return "fragment shader";
      case GL_COMPUTE_SHADER:
        return "compute shader";
    }
  }

}; // Shader

inline void
Shader::loadSourceFromFile(const char* fileName)
{
  if (fileName == nullptr)
    return;

  const auto* buffer = readShaderFile(fileName);

  // Set shader source code.
  glShaderSource(_handle, 1, &buffer, 0);
  _compiled = false;
  // Delete buffer.
  delete[] buffer;
  // Compile shader.
  compile();
}

inline void
Shader::setSource(const char* buffer)
{
  if (buffer == nullptr)
    return;
  // Set shader source code.
  glShaderSource(_handle, 1, &buffer, 0);
  // Compile shader.
  compile();
}

void
Shader::compile()
{
  // Compile shader.
  glCompileShader(_handle);

  GLint ok;

  // Get compile status.
  glGetShaderiv(_handle, GL_COMPILE_STATUS, &ok);
  if (ok == GL_TRUE)
    _compiled = true;
  else
  {
    auto log = infoLog(_handle, glGetShaderiv, glGetShaderInfoLog);
    error(COMPILE_ERROR, name(), log.c_str());
  }
}

/////////////////////////////////////////////////////////////////////
//
// Program implementation
// =======
Program* Program::_current;

Program::Program(const char* programName):
  _handle{0},
  _name{programName},
  _state{State::CREATED}
{
  // do nothing
}

Program::~Program()
{
  if (_handle != 0)
  {
    disuse();
    // Delete program.
    glDeleteProgram(_handle);
  }
}

Program&
Program::addShader(GLenum type, ShaderSource source, const char* buffer)
{
  if (_handle == 0)
    // Create program.
    _handle = glCreateProgram();

  Shader s{type};

  if (_state == State::IN_USE)
    error(CANNOT_ATTACH_SHADER, name(), s.name());
  if (source == ShaderSource::FILE)
    s.loadSourceFromFile(buffer);
  else
    s.setSource(buffer);
  // Attach shader.
  glAttachShader(_handle, s);
  _state = State::MODIFIED;
  return *this;
}

void
Program::use()
{
  switch (_state)
  {
    case State::IN_USE:
      break;
    case State::CREATED:
      error(CANNOT_USE_PROGRAM, name());
      break;
    case State::MODIFIED:
      link();
    case State::BUILT:
      if (_current != nullptr)
        _current->_state = State::BUILT;
      glUseProgram(_handle);
      _state = State::IN_USE;
      _current = this;
  }
}

inline void
Program::checkInUse() const
{
  if (_state != State::IN_USE)
    error(PROGRAM_NOT_IN_USE, name());
}

void
Program::disuse()
{
  if (_state == State::IN_USE)
  {
    _current = nullptr;
    glUseProgram(0);
    _state = State::BUILT;
  }
}

GLint
Program::attributeLocation(const char* s) const
{
  checkInUse();

  auto loc = glGetAttribLocation(_handle, s);

  if (loc == -1)
    error(VARIABLE_NOT_FOUND, name(), s);
  return loc;
}

GLint
Program::uniformLocation(const char* s) const
{
  checkInUse();

  auto loc = glGetUniformLocation(_handle, s);

  if (loc == -1)
    error(VARIABLE_NOT_FOUND, name(), s);
  return loc;
}

GLuint
Program::subroutineIndex(GLenum shader, const char* s) const
{
  checkInUse();

  auto index = glGetSubroutineIndex(_handle, shader, s);

  if (index == GL_INVALID_INDEX)
    error(SUBROUTINE_NOT_FOUND, name(), s);
  return index;
}

void
Program::link()
{
  // Link program
  glLinkProgram(_handle);

  GLint ok;

  // Get link status
  glGetProgramiv(_handle, GL_LINK_STATUS, &ok);
  if (ok == GL_TRUE)
    _state = State::BUILT;
  else
  {
    auto log = infoLog(_handle, glGetProgramiv, glGetProgramInfoLog);
    error(LINK_ERROR, name(), log.c_str());
  }
}

} // end namespace GLSL

} // end namespace cg
