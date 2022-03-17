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
// OVERVIEW: PolyMeshBuilder.cpp
// ========
// Source file for vis poly mesh builder.
//
// Author: Paulo Pagliosa
// Last revision: 16/03/2022

#include "PolyMeshBuilder.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// PolyMeshBuilder implementation
// ===============
void
PolyMeshBuilder::makeInstance(const PolyMeshGeometry& geometry)
{
  PolyMesh::Instance i{&geometry,
    _localToWorld,
    normalMatrix(),
    _mesh->instanceColor};

  _mesh->_instances.add(i);
  _mesh->_bounds.inflate(i.bounds());
}

} // end namespace cg::vis