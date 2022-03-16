//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: TriCellMeshMapper.cpp
// ========
// Source file for vis triangle mesh mapper.
//
// Author: Paulo Pagliosa
// Last revision: 15/03/2022

#include "graphics/GLRenderer.h"
#include "TriCellMeshMapper.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// TriCellMeshMapper implementation
// =================
const char*
TriCellMeshMapper::name() const
{
  return "TriCellMesh Mapper";
}

inline void
TriCellMeshMapper::renderColorMap() const
{
  if (auto nc = (uint32_t)colorMap().size())
  {
    if (_colorBuffer == nullptr)
      _colorBuffer = new GLColorBuffer{nc};
    else
    {
      _colorBuffer->bind();
      if (_colorBuffer->size() != nc)
        _colorBuffer->resize(nc);
    }
    _colorBuffer->setData(colorMap().data());
  }
}

bool
TriCellMeshMapper::mapColors(Scalars* scalars) const
{
  return Base::mapColors(scalars) ? void(renderColorMap()), true : false;
}

bool
TriCellMeshMapper::draw(GLRenderer& renderer) const
{
  renderer.flags.enable(GLRenderer::UseVertexColors,
    _colorBuffer != nullptr && useVertexColors);
  if (auto m = glMesh(input()->mesh()))
  {
    m->setColors(_colorBuffer, 2);
    renderer.drawMesh(*input());
  }
  return true;
}

} // end namespace cg::vis
