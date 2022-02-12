//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2020 Paulo Pagliosa.                        |
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
// OVERVIEW: GLBufer.h
// ========
// Class definition for OpenGL buffer.
//
// Author: Paulo Pagliosa
// Last revision: 06/07/2020

#ifndef __GLBuffer_h
#define __GLBuffer_h

#include "core/SharedObject.h"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#define NOMINMAX
#include <GL/gl3w.h>
#endif
#include <cstdint>
#include <iostream>
#include <stdexcept>

namespace cg
{ // begin namespace cg

#define DFL_GL_MAP_ACCESS (GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT)


/////////////////////////////////////////////////////////////////////
//
// GLBuffer: GL buffer class
// ========
template <typename T>
class GLBuffer: public SharedObject
{
public:
  ~GLBuffer() override
  {
    glDeleteBuffers(1, &_buffer);
  }

  GLBuffer(uint32_t size, GLenum target = GL_ARRAY_BUFFER)
  {
    if (size == 0)
      throw std::runtime_error("GLBuffer(): bad size");
    glGenBuffers(1, &_buffer);
    _target = target;
    bind();
    resize(size);
  }

  void bind()
  {
    glBindBuffer(_target, _buffer);
  }

  void unbind()
  {
    glBindBuffer(_target, 0);
  }

  void resize(uint32_t size);

  T* map(uint32_t first, uint32_t count, GLbitfield = DFL_GL_MAP_ACCESS);

  T* map(GLbitfield access = DFL_GL_MAP_ACCESS)
  {
    return map(0, _size, access);
  }

  void unmap()
  {
    glUnmapBuffer(_target);
  }

  void setData(uint32_t first, uint32_t count, const T* data);

  void setData(const T* data)
  {
    setData(0, _size, data);
  }

  operator GLuint()
  {
    return _buffer;
  }

  uint32_t size() const
  {
    return _size;
  }

  void dump(std::ostream& out = std::cout);

private:
  GLenum _target;
  uint32_t _size;
  GLuint _buffer;

}; // GLShader

template <typename T>
inline void
GLBuffer<T>::resize(uint32_t size)
{
  glBufferData(_target, size * sizeof(T), nullptr, GL_DYNAMIC_DRAW);
  _size = size;
}

template <typename T>
inline T*
GLBuffer<T>::map(uint32_t first, uint32_t count, GLbitfield access)
{
  bind();
  return static_cast<T*>(glMapBufferRange(_target,
    first * sizeof(T),
    count * sizeof(T),
    access));
}

template <typename T>
inline void
GLBuffer<T>::setData(uint32_t first, uint32_t count, const T* data)
{
  glBufferSubData(_target, first * sizeof(T), count * sizeof(T), data);
}

template <typename T>
void
GLBuffer<T>::dump(std::ostream& out)
{
  T* data = map(GL_MAP_READ_BIT);

  for (uint32_t i = 0; i < _size; ++i)
    out << i << ": " << data[i] << '\n';
  unmap();
}

} // end namespace cg

#endif // __GLBuffer_h
