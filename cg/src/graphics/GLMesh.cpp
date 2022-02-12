//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2020 Paulo Pagliosa.                        |
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
// OVERVIEW: GLMesh.cpp
// ========
// Source file for OpenGL mesh array object.
//
// Author: Paulo Pagliosa
// Last revision: 15/06/2020

#include "graphics/GLMesh.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLMesh implementation
// ======
GLMesh::GLMesh(const TriangleMesh& mesh)
{
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  glGenBuffers(4, _buffers);

  const auto& m = mesh.data();

  if (auto s = size<vec3f>(m.vertexCount))
  {
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, s, m.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, s, m.vertexNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
  }
  if (auto s = mesh.hasUV() * size<vec2f>(m.vertexCount))
  {
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, s, m.uv, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
  }
  if (auto s = size<TriangleMesh::Triangle>(m.triangleCount))
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, s, m.triangles, GL_STATIC_DRAW);
  }
  _vertexCount = m.triangleCount * 3;
}

void
GLMesh::setColors(GLColorBuffer* colors, int location)
{
  bind();
  if (colors != nullptr)
  {
    colors->bind();
    glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
  }
  glVertexAttrib4f(location, 0, 0, 0, 0);
}

} // end namespace cg
