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
// OVERVIEW: GLPoints3.h
// ========
// Class definition for OpenGL 3D point buffer object.
//
// Author: Paulo Pagliosa
// Last revision: 29/08/2023

#ifndef __GLPoints3_h
#define __GLPoints3_h

#include "graphics/Color.h"
#include "graphics/GLBuffer.h"
#include "math/Vector3.h"
#include <vector>

namespace cg
{ // begin namespace cg

class GLPoints3Renderer;
class GLRenderer;

using GLColorBuffer = GLBuffer<Color>;


/////////////////////////////////////////////////////////////////////
//
// GLPoints3: OpenGL 3D point buffer object class
// =========
class GLPoints3: public SharedObject
{
public:
  using PointArray = std::vector<vec3f>;

  GLPoints3(const PointArray& points);

  ~GLPoints3()
  {
    glDeleteBuffers(1, &_buffer);
    glDeleteVertexArrays(1, &_vao);
  }

  auto size() const
  {
    return _size;
  }

  void bind()
  {
    glBindVertexArray(_vao);
  }

  void setColors(GLColorBuffer* colors, int location = 1);

private:
  GLuint _vao;
  GLuint _buffer;
  uint32_t _size;

  friend GLPoints3Renderer;
  friend GLRenderer;

}; // GLPoints3

} // end namespace cg

#endif // __GLPoints3_h
