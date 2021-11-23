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
// OVERVIEW: GLProgram.h
// ========
// Class definition for GLSL program.
//
// Author: Paulo Pagliosa
// Last revision: 10/08/2018

#ifndef __GLProgram_h
#define __GLProgram_h

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#define NOMINMAX
#include <GL/gl3w.h>
#endif
#include <GLFW/glfw3.h>
#include <string>

namespace cg
{ // begin namespace cg

namespace GLSL
{ // begin namespace GLSL

enum class ShaderSource
{
  FILE,
  STRING
};


/////////////////////////////////////////////////////////////////////
//
// Program: GLSL program class
// =======
class Program
{
public:
  enum class State
  {
    CREATED,
    MODIFIED,
    BUILT,
    IN_USE
  };

  // Constructs an intance of Program.
  Program(const char*);

  Program(const Program&) = delete;
  Program& operator =(const Program&) = delete;

  // Destructor.
  ~Program();

  // Casts to handle type.
  operator GLuint() const
  {
    return _handle;
  }

  // Returns the name of this program.
  auto name() const
  {
    return _name.c_str();
  }

  // Returns the state of this program.
  State state() const
  {
    return _state;
  }

  // Adds a shader in this program.
  Program& addShader(GLenum, ShaderSource, const char*);

  Program& setShader(GLenum type, const char* code)
  {
    return addShader(type, ShaderSource::STRING, code);
  }

  Program& loadShader(GLenum type, const std::string& filename)
  {
    return addShader(type, ShaderSource::FILE, filename.c_str());
  }

  // Adds vertex and fragment shaders in this program.
  Program& setShaders(const char* vs, const char* fs)
  {
    setShader(GL_VERTEX_SHADER, vs);
    return setShader(GL_FRAGMENT_SHADER, fs);
  }

  Program& loadShaders(const std::string& vs, const std::string& fs)
  {
    loadShader(GL_VERTEX_SHADER, vs);
    return loadShader(GL_FRAGMENT_SHADER, fs);
  }

  // Uses/disuses this program.
  void use();
  void disuse();

  // Gets uniform variable location.
  GLint uniformLocation(const char*) const;

  // Sets uniform variable by location.
  static void setUniform(GLint, GLint);
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

  // Sets uniform variable by name.
  void setUniform(const char*, GLint);
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

  // Gets atributte location.
  GLint attributeLocation(const char*) const;

  // Gets subroutine index.
  GLuint subroutineIndex(GLenum, const char*) const;
  GLuint vertexSubroutineIndex(const char*) const;
  GLuint fragmentSubroutineIndex(const char*) const;

  // Sets subroutine by index.
  static void setSubroutine(GLenum, GLuint&);
  static void setVertexSubroutine(GLuint&);
  static void setFragmentSubroutine(GLuint&);

  // Sets subroutine by name.
  void setSubroutine(GLenum, const char*);
  void setVertexSubroutine(const char*);
  void setFragmentSubroutine(const char*);

  // Gets the current program.
  static auto current()
  {
    return _current;
  }

  // Sets the current program.
  static void setCurrent(Program* program)
  {
    if (_current != program)
      program == nullptr ? _current->disuse() : program->use();
  }

protected:
  // Link this program.
  void link();

private:
  static Program* _current;

  GLuint _handle;
  std::string _name;
  State _state;

  // Check if this program is in use.
  void checkInUse() const;

}; // Program

inline void
Program::setUniform(GLint loc, GLint i0)
{
  glUniform1i(loc, i0);
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
Program::setUniform(const char* name, GLint i0)
{
  setUniform(uniformLocation(name), i0);
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

} // end namespace GLSL

} // end namespace cg

#endif // __GLProgram_h
