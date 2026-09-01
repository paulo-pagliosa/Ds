//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2026 Paulo Pagliosa.                        |
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
// Last revision: 31/08/2026

#include "graphics/GLProgram.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace cg::GLSL
{ // begin namespace cg::GLSL

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
error(int code, ...)
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
class Shader: public NamedObject
{
public:
  /// Constructs a Shader.
  Shader(GLenum shaderType):
    NamedObject{shaderName(shaderType)},
    _handle{glCreateShader(shaderType)}
  {
    // do nothing
  }

  /// Destructor.
  ~Shader()
  {
    glDeleteShader(_handle);
  }

  /// Sets the source of this shader from a file.
  void loadSourceFromFile(const char*);

  /// Sets the source of this shader from a string.
  void setSource(const char*);

  // Returns the id of this shader.
  [[nodiscard]] operator GLuint() const
  {
    return _handle;
  }

  // Returns true if this shader is compiled.
  [[nodiscard]] bool isCompiled() const
  {
    return _compiled;
  }

private:
  GLuint _handle;
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
Shader::loadSourceFromFile(const char* filename)
{
  if (!filename)
    return;

  const auto* buffer = readShaderFile(filename);

  // Set the shader source code.
  glShaderSource(_handle, 1, &buffer, 0);
  _compiled = false;
  // Delete the source code buffer.
  delete[] buffer;
  // Compile the shader.
  compile();
}

inline void
Shader::setSource(const char* buffer)
{
  if (!buffer)
    return;
  // Set the shader source code.
  glShaderSource(_handle, 1, &buffer, 0);
  // Compile the shader.
  compile();
}

void
Shader::compile()
{
  // Compile the shader.
  glCompileShader(_handle);

  GLint ok;

  // Check for compilation errors.
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

Program::Program(const char* name):
  NamedObject{name},
  _handle{0},
  _state{State::CREATED}
{
  // do nothing
}

Program::~Program()
{
  if (_handle)
  {
    disuse();
    // Delete the program.
    glDeleteProgram(_handle);
  }
}

Program&
Program::addShader(GLenum type, ShaderSource source, const char* buffer)
{
  if (!_handle)
    // Create the program.
    _handle = glCreateProgram();

  Shader s{type};

  if (_state == State::IN_USE)
    error(CANNOT_ATTACH_SHADER, name(), s.name());
  if (source == ShaderSource::FILE)
    s.loadSourceFromFile(buffer);
  else
    s.setSource(buffer);
  // Attach the shader.
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
      [[fallthrough]];
    case State::BUILT:
      if (_current)
        _current->_state = State::BUILT;
      glUseProgram(_handle);
      _state = State::IN_USE;
      _current = this;
  }
}

void
Program::assertInUse() const
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
Program::uniformLocation(const char* s) const
{
  assertInUse();

  auto loc = glGetUniformLocation(_handle, s);

  if (loc == -1)
    error(VARIABLE_NOT_FOUND, name(), s);
  return loc;
}

GLint
Program::attributeLocation(const char* s) const
{
  assertInUse();

  auto loc = glGetAttribLocation(_handle, s);

  if (loc == -1)
    error(VARIABLE_NOT_FOUND, name(), s);
  return loc;
}

GLuint
Program::subroutineIndex(GLenum shader, const char* s) const
{
  assertInUse();

  auto index = glGetSubroutineIndex(_handle, shader, s);

  if (index == GL_INVALID_INDEX)
    error(SUBROUTINE_NOT_FOUND, name(), s);
  return index;
}

void
Program::link()
{
  // Link the program.
  glLinkProgram(_handle);

  GLint ok;

  // Check for linking errors.
  glGetProgramiv(_handle, GL_LINK_STATUS, &ok);
  if (ok == GL_TRUE)
    _state = State::BUILT;
  else
  {
    auto log = infoLog(_handle, glGetProgramiv, glGetProgramInfoLog);
    error(LINK_ERROR, name(), log.c_str());
  }
}

} // end namespace cg::GLSL
