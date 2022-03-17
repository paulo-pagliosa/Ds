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
// OVERVIEW: PolyMeshBuilder.h
// ========
// Class definition for vis poly mesh builder.
//
// Author: Paulo Pagliosa
// Last revision: 15/03/2022

#ifndef __PolyMeshBuilder_h
#define __PolyMeshBuilder_h

#include "PolyMesh.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// PolyMeshBuilder: vis poly mesh buider class
// ===============
class PolyMeshBuilder: public PolyMeshHelper
{
public:
  PolyMesh* mesh() const
  {
    return _mesh;
  }

  void makeInstance(const PolyMeshGeometry&);

protected:
  Reference<PolyMesh> _mesh;

  PolyMeshBuilder():
    _mesh{new PolyMesh}
  {
    // do nothing
  }

}; // PolyMeshBuilder

} // end namespace cg::vis

#endif // __PolyMeshBuilder_h
