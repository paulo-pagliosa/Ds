//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: AssetFolder.h
// ========
// Class definition for asset folder.
//
// Author: Paulo Pagliosa
// Last revision: 11/07/2023

#ifndef __AssetFolder_h
#define __AssetFolder_h

#include "graphics/Assets.h"
#include <cassert>
#include <filesystem>
#include <set>

namespace cg
{ // begin namespace cg

namespace fs = std::filesystem;

class AssetFolder;
using AssetFolderRef = Reference<AssetFolder>;

template <typename T>
class RefCmp
{
public:
  using Ref = Reference<T>;

  auto operator ()(const Ref& lhs, const Ref& rhs) const
  {
    return std::less<T>{}(*lhs, *rhs);
  }

}; // RefCmp

template <typename T>
using RefSet = std::set<Reference<T>, RefCmp<T>>;
template <typename T>
using RefSetIterator = typename RefSet<T>::const_iterator;


/////////////////////////////////////////////////////////////////////
//
// AssetFile: asset file class
// =========
class AssetFile: public NameableObject
{
public:
  AssetFolder* parent() const
  {
    return _parent;
  }

  auto& filename() const
  {
    return _filename;
  }

  auto operator <(const AssetFile& other) const
  {
    return _filename < other._filename;
  }

private:
  AssetFolder* _parent;
  std::string _filename;

  AssetFile(const char* filename, AssetFolder* parent = nullptr):
    _filename{filename},
    _parent{parent}
  {
    assert(filename && *filename);
  }

  static AssetFile* make(const char*, AssetFolder*);

  friend AssetFolder;

}; // AssetFile


/////////////////////////////////////////////////////////////////////
//
// AssetFolder: asset folder class
// ===========
class AssetFolder: public AssetFile
{
public:
  static AssetFolderRef New(const char* name, const char* ext = nullptr);

  auto& path() const
  {
    return _path;
  }

  auto empty() const
  {
    return _folders.empty() && _files.empty();
  }

  auto folderCount() const
  {
    return int(_folders.size());
  }

  auto& folders() const
  {
    return _folders;
  }

  auto findFolder(const char* name) const
  {
    return find(_folders, name);
  }

  auto fileCount() const
  {
    return int(_files.size());
  }

  auto& files() const
  {
    return _files;
  }

  auto findFile(const char* filename) const
  {
    return find(_files, filename);
  }

  bool addFile(const char* filename);

private:
  RefSet<AssetFolder> _folders;
  RefSet<AssetFile> _files;
  fs::path _path;

  using AssetFile::AssetFile;
  void initialize(const fs::path&, const char*);

  static AssetFolder* make(const char*, AssetFolder* = nullptr);

  template <typename T>
  static T* find(const RefSet<T>& set, const char* name)
  {
    const T temp{name};
    auto found = set.find(T::makeUse(&temp));

    return set.end() == found ? nullptr : *found;
  }

}; // AssetFolder

} // end namespace cg

#endif // __AssetFolder_h
