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
// OVERVIEW: GLProgram.h
// ========
// Class definition for GLSL program.
//
// Author: Paulo Pagliosa
// Last revision: 31/08/2026

#ifndef __GLProgram_h
#define __GLProgram_h

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#define NOMINMAX
#include "GL/gl3w.h"
#endif
#include "GLFW/glfw3.h"
#include "core/NamedObject.h"

namespace cg::GLSL
{ // begin namespace cg::GLSL

enum class ShaderSource
{
  FILE,
  STRING
};


/////////////////////////////////////////////////////////////////////
//
// Program: GLSL program class
// =======
class Program: public NamedObject
{
public:
  enum class State
  {
    CREATED,
    MODIFIED,
    BUILT,
    IN_USE
  };

  /// Constructs a Program.
  Program(const char*);

  Program(const Program&) = delete;
  Program& operator =(const Program&) = delete;

  /// Destructor.
  ~Program();

  /// Returns tho id of this program.
  [[nodiscard]] operator GLuint() const
  {
    return _handle;
  }

  /// Returns the state of this program.
  [[nodiscard]] auto state() const
  {
    return _state;
  }

  /// Adds a shader in this program.
  Program& addShader(GLenum, ShaderSource, const char*);

  auto& setShader(GLenum type, const char* code)
  {
    return addShader(type, ShaderSource::STRING, code);
  }

  auto& loadShader(GLenum type, const std::string& filename)
  {
    return addShader(type, ShaderSource::FILE, filename.c_str());
  }

  /// Adds vertex and fragment shaders in this program.
  auto& setShaders(const char* vs, const char* fs)
  {
    setShader(GL_VERTEX_SHADER, vs);
    return setShader(GL_FRAGMENT_SHADER, fs);
  }

  auto& loadShaders(const std::string& vs, const std::string& fs)
  {
    loadShader(GL_VERTEX_SHADER, vs);
    return loadShader(GL_FRAGMENT_SHADER, fs);
  }

  /// Uses/disuses this program.
  void use();
  void disuse();

  /// Gets a uniform variable location of this program.
  [[nodiscard]] GLint uniformLocation(const char*) const;

  /// Asserts this program is in use.
  void assertInUse() const;

  /// Sets a uniform variable of this program by location.
  static void setUniform(GLint, int);
  static void setUniform(GLint, unsigned);
  static void setUniform(GLint, float);
  static void setUniform(GLint, float, float);
  static void setUniform(GLint, float, float, float);
  static void setUniform(GLint, float, float, float, float);
  template<typename vec2f> static void setUniformVec2(GLint, const vec2f&);
  template<typename vec3f> static void setUniformVec3(GLint, const vec3f&);
  template<typename vec4f> static void setUniformVec4(GLint, const vec4f&);
  template<typename mat2f> static void setUniformMat2(GLint, const mat2f&);
  template<typename mat3f> static void setUniformMat3(GLint, const mat3f&);
  template<typename mat4f> static void setUniformMat4(GLint, const mat4f&);

  /// Sets a uniform variable of this program by name.
  void setUniform(const char*, int);
  void setUniform(const char*, unsigned);
  void setUniform(const char*, float);
  void setUniform(const char*, float, float);
  void setUniform(const char*, float, float, float);
  void setUniform(const char*, float, float, float, float);
  template<typename vec2f> void setUniformVec2(const char*, const vec2f&);
  template<typename vec3f> void setUniformVec3(const char*, const vec3f&);
  template<typename vec4f> void setUniformVec4(const char*, const vec4f&);
  template<typename mat2f> void setUniformMat2(const char*, const mat2f&);
  template<typename mat3f> void setUniformMat3(const char*, const mat3f&);
  template<typename mat4f> void setUniformMat4(const char*, const mat4f&);

  // Gets an atributte location of this program.
  [[nodiscard]] GLint attributeLocation(const char*) const;

  /// Gets a subroutine index of this program.
  [[nodiscard]] GLuint subroutineIndex(GLenum, const char*) const;
  [[nodiscard]] GLuint vertexSubroutineIndex(const char*) const;
  [[nodiscard]] GLuint fragmentSubroutineIndex(const char*) const;

  /// Sets a subroutine of this program by index.
  static void setSubroutine(GLenum, GLuint&);
  static void setVertexSubroutine(GLuint&);
  static void setFragmentSubroutine(GLuint&);

  /// Sets a ubroutine of thi program by name.
  void setSubroutine(GLenum, const char*);
  void setVertexSubroutine(const char*);
  void setFragmentSubroutine(const char*);

  /// Gets the current program.
  [[nodiscard]] static auto current()
  {
    return _current;
  }

  /// Sets the current program.
  static void setCurrent(Program* program)
  {
    if (_current != program)
      program ? program->use() : _current->disuse();
  }

protected:
  // Link this program.
  void link();

private:
  static Program* _current;

  GLuint _handle;
  State _state;

}; // Program

inline void
Program::setUniform(GLint loc, int i0)
{
  glUniform1i(loc, i0);
}

inline void
Program::setUniform(GLint loc, unsigned u0)
{
  glUniform1ui(loc, u0);
}

inline void
Program::setUniform(GLint loc, float f0)
{
  glUniform1f(loc, f0);
}

inline void
Program::setUniform(GLint loc, float f0, float f1)
{
  glUniform2f(loc, f0, f1);
}

inline void
Program::setUniform(GLint loc, float f0, float f1, float f2)
{
  glUniform3f(loc, f0, f1, f2);
}

inline void
Program::setUniform(GLint loc, float f0, float f1, float f2, float f3)
{
  glUniform4f(loc, f0, f1, f2, f3);
}

template<typename vec2f>
inline void
Program::setUniformVec2(GLint loc, const vec2f& v)
{
  glUniform2fv(loc, 1, (float*)&v);
}

template<typename vec3f>
inline void
Program::setUniformVec3(GLint loc, const vec3f& v)
{
  glUniform3fv(loc, 1, (float*)&v);
}

template<typename vec4f>
inline void
Program::setUniformVec4(GLint loc, const vec4f& v)
{
  glUniform4fv(loc, 1, (float*)&v);
}

template<typename mat2f>
inline void
Program::setUniformMat2(GLint loc, const mat2f& m)
{
  glUniformMatrix2fv(loc, 1, GL_FALSE, (float*)&m);
}

template<typename mat3f>
inline void
Program::setUniformMat3(GLint loc, const mat3f& m)
{
  glUniformMatrix3fv(loc, 1, GL_FALSE, (float*)&m);
}

template<typename mat4f>
inline void
Program::setUniformMat4(GLint loc, const mat4f& m)
{
  glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&m);
}

inline void
Program::setUniform(const char* name, int i0)
{
  setUniform(uniformLocation(name), i0);
}

inline void
Program::setUniform(const char* name, unsigned u0)
{
  setUniform(uniformLocation(name), u0);
}

inline void
Program::setUniform(const char* name, float f0)
{
  setUniform(uniformLocation(name), f0);
}

inline void
Program::setUniform(const char* name, float f0, float f1)
{
  setUniform(uniformLocation(name), f0, f1);
}

inline void
Program::setUniform(const char* name, float f0, float f1, float f2)
{
  setUniform(uniformLocation(name), f0, f1, f2);
}

inline void
Program::setUniform(const char* name, float f0, float f1, float f2, float f3)
{
  setUniform(uniformLocation(name), f0, f1, f2, f3);
}

template<typename vec2f>
inline void
Program::setUniformVec2(const char* name, const vec2f& v)
{
  setUniformVec2(uniformLocation(name), v);
}

template<typename vec3f>
inline void
Program::setUniformVec3(const char* name, const vec3f& v)
{
  setUniformVec3(uniformLocation(name), v);
}

template<typename vec4f>
inline void
Program::setUniformVec4(const char* name, const vec4f& v)
{
  setUniformVec4(uniformLocation(name), v);
}

template<typename mat2f>
inline void
Program::setUniformMat2(const char* name, const mat2f& m)
{
  setUniformMat2(uniformLocation(name), m);
}

template<typename mat3f>
inline void
Program::setUniformMat3(const char* name, const mat3f& m)
{
  setUniformMat3(uniformLocation(name), m);
}

template<typename mat4f>
inline void
Program::setUniformMat4(const char* name, const mat4f& m)
{
  setUniformMat4(uniformLocation(name), m);
}

inline GLuint
Program::vertexSubroutineIndex(const char* name) const
{
  return subroutineIndex(GL_VERTEX_SHADER, name);
}

inline GLuint
Program::fragmentSubroutineIndex(const char* name) const
{
  return subroutineIndex(GL_FRAGMENT_SHADER, name);
}

inline void
Program::setSubroutine(GLenum shader, GLuint& index)
{
  glUniformSubroutinesuiv(shader, 1, &index);
}

inline void
Program::setVertexSubroutine(GLuint& index)
{
  setSubroutine(GL_VERTEX_SHADER, index);
}

inline void
Program::setFragmentSubroutine(GLuint& index)
{
  setSubroutine(GL_FRAGMENT_SHADER, index);
}

inline void
Program::setSubroutine(GLenum shader, const char* name)
{
  GLuint index = subroutineIndex(shader, name);
  setSubroutine(shader, index);
}

inline void
Program::setVertexSubroutine(const char* name)
{
  setSubroutine(GL_VERTEX_SHADER, name);
}

inline void
Program::setFragmentSubroutine(const char* name)
{
  setSubroutine(GL_FRAGMENT_SHADER, name);
}

} // end namespace cg::GLSL

#endif // __GLProgram_h
