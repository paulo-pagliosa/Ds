//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2023 Paulo Pagliosa.                              |
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
// OVERVIEW: GLPoints3.cpp
// ========
// Source file for OpenGL 3D point buffer object.
//
// Author: Paulo Pagliosa
// Last revision: 29/08/2023

#include "graphics/GLPoints3.h"
#include <cassert>

namespace cg
{ // begin namespace cg

template <typename T>
inline auto bufferSize(size_t n)
{
  return sizeof(T) * n;
}


/////////////////////////////////////////////////////////////////////
//
// GLPoints3 implementation
// =========
GLPoints3::GLPoints3(const PointArray& points):
  _size{uint32_t(points.size())}
{
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  glGenBuffers(1, &_buffer);
  if (auto s = bufferSize<vec3f>(_size))
  {
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glBufferData(GL_ARRAY_BUFFER, s, points.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
  }
}

void
GLPoints3::setColors(GLColorBuffer* colors, int location)
{
  bind();
  if (colors != nullptr)
  {
    assert(colors->size() == _size);
    colors->bind();
    glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
  }
  glVertexAttrib4f(location, 0, 0, 0, 0);
}

} // end namespace cg
