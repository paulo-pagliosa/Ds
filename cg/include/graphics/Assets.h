//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Orthrus Group.                         |
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
// OVERVIEW: Assets.h
// ========
// Class definition for assets.
//
// Author: Paulo Pagliosa
// Last revision: 21/19/2022

#ifndef __Assets_h
#define __Assets_h

#include "graphics/Material.h"
#include "utils/MeshReader.h"
#include <map>
#include <string>

namespace cg
{ // begin namespace cg

using MeshRef = Reference<TriangleMesh>;
using MeshMap = std::map<std::string, MeshRef>;
using MeshMapIterator = typename MeshMap::const_iterator;
using MaterialRef = Reference<Material>;
using MaterialMap = std::map<std::string, MaterialRef>;
using MaterialMapIterator = typename MaterialMap::const_iterator;


/////////////////////////////////////////////////////////////////////
//
// Assets: assets class
// ======
class Assets
{
public:
  static void initialize();

  static MeshMap& meshes()
  {
    return _meshes;
  }

  static TriangleMesh* loadMesh(MeshMapIterator mit);

  static MaterialMap& materials()
  {
    return _materials;
  }

private:
  static MeshMap _meshes;
  static MaterialMap _materials;

}; // Assets

} // end namespace cg

#endif // __Assets_h
