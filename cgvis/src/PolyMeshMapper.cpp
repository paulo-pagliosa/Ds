//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Paulo Pagliosa.                              |
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
// OVERVIEW: PolyMeshMapper.cpp
// ========
// Source file for vis poly mesh mapper.
//
// Author: Paulo Pagliosa
// Last revision: 16/03/2022

#include "PolyMeshMapper.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// PolyMeshMapper implementation
// ==============
const char*
PolyMeshMapper::name() const
{
  return "PolyMesh Mapper";
}

inline void
PolyMeshMapper::draw(GLRenderer& renderer, const PolyMesh::Instance& instance)
{
  for (const auto& e : instance.geometry->elements())
  {
    const auto m = instance.localToWorld * e.localToWorld;
    const auto n = instance.normalMatrix * e.normalMatrix;

    renderer.drawMesh(*e.mesh, m, n);
  }
}

bool
PolyMeshMapper::draw(GLRenderer& renderer) const
{
  const auto& colorMap = this->colorMap();
  auto useColorMap = !colorMap.empty() && useVertexColors;
  auto id = 0;

  for (const auto& instance : input()->instances())
  {
    if (useColorMap)
      renderer.setMeshColor(colorMap[id]);
    else
      renderer.setMeshColor(instance.color);
    draw(renderer, instance);
    ++id;
  }
  return true;
}

} // end namespace cg::vis
