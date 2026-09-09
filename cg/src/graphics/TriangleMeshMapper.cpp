//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: TriangleMeshMapper.cpp
// ========
// Source file for triangle mesh mapper.
//
// Author: Paulo Pagliosa
// Last revision: 09/09/2026

#include "graphics/TriangleMeshMapper.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// TriangleMeshMapper implementation
// ==================
TriangleMeshMapper::TriangleMeshMapper(const TriangleMesh& mesh)
{
  _shape = new TriangleMeshShape{mesh};
  _primitive = new ShapeInstance{*_shape};
}

void
TriangleMeshMapper::setMesh(const TriangleMesh& mesh)
{
  _shape->setMesh(mesh);
}

bool
TriangleMeshMapper::render(GLRenderer& renderer) const
{
  auto m = renderer.renderMode();

  if (m == renderMode || !useObjectRenderMode)
    return renderer.drawMesh(*_primitive);
  renderer.setRenderMode(renderMode);

  auto r = renderer.drawMesh(*_primitive);

  renderer.setRenderMode(m);
  return r;
}

Bounds3f
TriangleMeshMapper::bounds() const
{
  return _primitive->bounds();
}

Primitive*
TriangleMeshMapper::primitive() const
{
  return _primitive;
}

} // end namespace cg
