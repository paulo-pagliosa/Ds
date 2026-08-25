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
// OVERVIEW: PolyMesh.cpp
// ========
// Source file for vis poly mesh.
//
// Author: Paulo Pagliosa
// Last revision: 24/08/2026

#include "PolyMesh.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// PolyMesh implementation
// ========
Bounds3f
PolyMesh::Instance::bounds() const
{
  Bounds3f b;

  for (const auto& e : geometry->elements())
    b.extend({e.bounds(), localToWorld});
  return b;
}

} // end namespace cg::vis
