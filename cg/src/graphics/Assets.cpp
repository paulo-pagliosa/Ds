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
// OVERVIEW: Assets.cpp
// ========
// Source file for assets.
//
// Author: Paulo Pagliosa
// Last revision: 03/02/2022

#include "graphics/Application.h"
#include "graphics/Assets.h"
#include <filesystem>

namespace cg
{ // begin namespace cg

namespace fs = std::filesystem;


/////////////////////////////////////////////////////////////////////
//
// Assets implementation
// ======
bool Assets::_initialized;
MeshMap Assets::_meshes;
MaterialMap Assets::_materials;
size_t Assets::_maxMeshSize;
size_t Assets::_meshSize;

static inline auto
meshSize(const TriangleMesh* m)
{
  return m->data().vertexCount;
}

static inline auto
unmapUnusedMeshes(MeshMap& meshes)
{
  size_t ms{};

  for (auto& [name, mesh] : meshes)
    if (mesh != nullptr)
      if (mesh->referenceCount() > 1)
        ms += meshSize(mesh);
      else
      {
#ifdef _DEBUG
        printf("**Releasing mesh '%s'...\n", name.c_str());
#endif // _DEBUG
        mesh = nullptr;
      }
  return ms;
}

void
Assets::initialize(size_t maxMeshSize)
{
  if (!_initialized)
  {
    fs::path mp{Application::assetFilePath("meshes/")};

    if (fs::is_directory(mp))
    {
      auto p = fs::directory_iterator(mp);

      for (auto e = fs::directory_iterator(); p != e; ++p)
        if (fs::is_regular_file(p->status()))
          _meshes[p->path().filename().string()] = nullptr;
    }

    auto dm = Material::defaultMaterial();

    _materials[dm->name()] = dm;
    _maxMeshSize = maxMeshSize ? maxMeshSize : dflMaxMeshSize;
    _initialized = true;
  }
}

TriangleMesh*
Assets::loadMesh(MeshMapIterator mit)
{
  if (mit == _meshes.end())
    return nullptr;

  TriangleMesh* m{mit->second};

  if (m == nullptr)
  {
    auto filename = "meshes/" + mit->first;

    if (m = Application::loadMesh(filename.c_str()))
    {
      auto s = meshSize(m);

      if (_meshSize + s > _maxMeshSize)
        // If unable to release any cached mesh, then delete
        // the last loaded mesh and return null
        if (auto ms = unmapUnusedMeshes(_meshes); ms != _meshSize)
          _meshSize = ms;
        else
        {
          delete m;
          return nullptr;
        }
      _meshSize += s;
      _meshes[mit->first] = m;
    }
  }
  return m;
}

} // end namespace cg
