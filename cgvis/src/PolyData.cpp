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
// OVERVIEW: PolyData.cpp
// ========
// Source file for vis poly data.
//
// Author: Paulo Pagliosa
// Last revision: 08/03/2022

#include "PolyData.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// PolyData implementation
// ========
void
PolyData::deleteAll()
{
  _points.clear();
  _lines.clear();
  _triangles.clear();
  _vertices.clear();
  _bounds.setEmpty();
  deleteLocator();
}

int
PolyData::addVertex(const vec3f& p)
{
  _bounds.inflate(p);
  if (_locator != nullptr)
    return _locator->add(p);
  _vertices.push_back(p);
  return (int)_vertices.size() - 1;
}

} // end namespace cg::vis
