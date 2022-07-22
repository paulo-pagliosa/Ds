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
// OVERVIEW: Assets.h
// ========
// Class definition for assets.
//
// Author: Paulo Pagliosa
// Last revision: 13/07/2022

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
  static constexpr auto dflMaxMeshSize = 1ULL << 20;

  static void initialize(size_t = dflMaxMeshSize);

  static MeshMap& meshes()
  {
    return _meshes;
  }

  static TriangleMesh* loadMesh(const std::string& meshName)
  {
    return loadMesh(_meshes.find(meshName));
  }

  static TriangleMesh* loadMesh(MeshMapIterator);

  static MaterialMap& materials()
  {
    return _materials;
  }

  static Material* findMaterial(const std::string& name)
  {
    if (auto mit = _materials.find(name); mit != _materials.end())
      return mit->second;
    return nullptr;
  }

private:
  static bool _initialized;
  static MeshMap _meshes;
  static MaterialMap _materials;
  static size_t _maxMeshSize;
  static size_t _meshSize;

}; // Assets

} // end namespace cg

#endif // __Assets_h
