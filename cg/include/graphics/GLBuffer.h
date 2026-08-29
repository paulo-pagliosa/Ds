//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2026 Paulo Pagliosa.                        |
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
// Last revision: 29/08/2026

#ifndef __GLBuffer_h
#define __GLBuffer_h

#include "core/SharedObject.h"
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#define NOMINMAX
#include <GL/gl3w.h>
#endif
#include <cassert>
#include <cstdint>
#include <iostream>
#include <map>

namespace cg
{ // begin namespace cg

[[nodiscard]] consteval GLbitfield
dflGLMapAccess()
{
  return GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT;
}


/////////////////////////////////////////////////////////////////////
//
// GLBufferBinder: GL buffer binder class
// ==============
class GLBufferBinder
{
public:
  static bool bind(GLenum target, GLuint buffer)
  {
    auto& b = _boundBuffers[target];
    return buffer == b ? false : (glBindBuffer(target, b = buffer), true);
  }

  static void unbind(GLenum target)
  {
    bind(target, 0);
  }

  [[nodiscard]] static auto currentBuffer(GLenum target)
  {
    return _boundBuffers[target];
  }

private:
  inline static std::map<GLenum, GLuint> _boundBuffers;

}; // GLBufferBinder


/////////////////////////////////////////////////////////////////////
//
// GLBuffer: GL buffer class
// ========
template <typename T>
class GLBuffer: public SharedObject
{
public:
  GLBuffer(uint32_t size, GLenum target = GL_ARRAY_BUFFER):
    _target{target},
    _size{}
  {
    glGenBuffers(1, &_buffer);
    bind();
    resize(size);
  }

  ~GLBuffer() override
  {
    assert(!_mapped);
    unbind();
    glDeleteBuffers(1, &_buffer);
  }

  bool bind()
  {
    return GLBufferBinder::bind(_target, _buffer);
  }

  void unbind()
  {
    GLBufferBinder::unbind(_target);
  }

  [[nodiscard]] bool bound() const
  {
    return GLBufferBinder::currentBuffer(_target) == _buffer;
  }

  bool resize(uint32_t size);

  [[nodiscard]] T* map(uint32_t first, uint32_t count, GLbitfield access);

  [[nodiscard]] T* map(GLbitfield access = dflGLMapAccess())
  {
    return map(0, _size, access);
  }

  void unmap()
  {
    if (_mapped)
    {
      assert(bound());
      glUnmapBuffer(_target);
      _mapped = false;
    }
  }

  void setData(uint32_t first, uint32_t count, const T* data);

  void setData(const T* data)
  {
    setData(0, _size, data);
  }

  [[nodiscard]] auto target() const
  {
    return _target;
  }

  [[nodiscard]] operator GLuint() const
  {
    return _buffer;
  }

  [[nodiscard]] auto size() const
  {
    return _size;
  }

  void dump(std::ostream& out = std::cout);

private:
  GLenum _target;
  GLuint _buffer;
  uint32_t _size;
  bool _mapped{};

}; // GLShader

template <typename T>
inline bool
GLBuffer<T>::resize(uint32_t size)
{
  assert(bound() && size > 0);
  if (_size == size)
    return false;
  glBufferData(_target, size * sizeof(T), nullptr, GL_DYNAMIC_DRAW);
  return _size = size;
}

template <typename T>
inline T*
GLBuffer<T>::map(uint32_t first, uint32_t count, GLbitfield access)
{
  if (_mapped)
    return nullptr;
  assert(bound() && first + count <= _size);

  auto data = static_cast<T*>(glMapBufferRange(_target,
    first * sizeof(T),
    count * sizeof(T),
    access));

  if (data)
    _mapped = true;
  return data;
}

template <typename T>
inline void
GLBuffer<T>::setData(uint32_t first, uint32_t count, const T* data)
{
  assert(bound() && first + count <= _size);
  glBufferSubData(_target, first * sizeof(T), count * sizeof(T), data);
}

template <typename T>
void
GLBuffer<T>::dump(std::ostream& out)
{
  if (auto data = map(GL_MAP_READ_BIT); !data)
    out << "Ubable to map buffer " << _buffer << '\n';
  else
  {
    for (uint32_t i = 0; i < _size; ++i)
      out << i << ": " << data[i] << '\n';
    unmap();
  }
}

} // end namespace cg

#endif // __GLBuffer_h
