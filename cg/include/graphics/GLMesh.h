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
// OVERVIEW: GLMesh.h
// ========
// Class definition for OpenGL mesh array object.
//
// Author: Paulo Pagliosa
// Last revision: 29/08/2026

#ifndef __GLMesh_h
#define __GLMesh_h

#include "geometry/TriangleMesh.h"
#include "graphics/GLBuffer.h"

namespace cg
{ // begin namespace cg

using GLColorBuffer = GLBuffer<Color>;


/////////////////////////////////////////////////////////////////////
//
// GLMesh: OpenGL mesh array object class
// ======
class GLMesh: public SharedObject
{
public:
  /// Constructs a GLMesh object.
  GLMesh(const TriangleMesh& mesh);

  /// Destructor.
  ~GLMesh()
  {
    glDeleteBuffers(4, _buffers);
    glDeleteVertexArrays(1, &_vao);
  }

  void bind()
  {
    glBindVertexArray(_vao);
  }

  [[nodiscard]] auto triangleCount() const
  {
    return _triangleCount;
  }

  void setColors(GLColorBuffer* colors);

private:
  constexpr static auto colorLoc = 3;

  GLuint _vao;
  GLuint _buffers[4];
  int _vertexCount;
  int _triangleCount;

}; // GLMesh

[[nodiscard]] inline GLMesh*
asGLMesh(SharedObject* object)
{
  return dynamic_cast<GLMesh*>(object);
}

[[nodiscard]] inline GLMesh*
glMesh(const TriangleMesh* mesh)
{
  if (nullptr == mesh)
    return nullptr;

  auto ma = asGLMesh(mesh->userData);

  if (nullptr == ma)
  {
    ma = new GLMesh{*mesh};
    mesh->userData = ma;
  }
  return ma;
}

} // end namespace cg

#endif // __GLMesh_h
