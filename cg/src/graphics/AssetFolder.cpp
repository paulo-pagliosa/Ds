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
// OVERVIEW: AssetFolder.cpp
// ========
// Source file for asset folder.
//
// Author: Paulo Pagliosa
// Last revision: 06/07/2023

#include "core/Exception.h"
#include "graphics/Application.h"
#include "graphics/AssetFolder.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// AssetFile implementation
// =========
AssetFile*
AssetFile::make(const char* filename, AssetFolder* parent)
{
  auto file = new AssetFile{filename, parent};
  
  file->setName("%s%s", parent ? parent->name() : "", filename);
  return file;
}


/////////////////////////////////////////////////////////////////////
//
// AssetFolder implementation
// ===========
AssetFolder*
AssetFolder::make(const char* name, AssetFolder* parent)
{
  auto file = new AssetFolder{name, parent};

  file->setName("%s%s/", parent ? parent->name() : "", name);
  return file;
}

AssetFolderRef
AssetFolder::New(const char* name, const char* ext)
{
  fs::path path{Application::assetFilePath(name)};

  if (!fs::exists(path))
    fs::create_directory(path);
  else if (!fs::is_directory(path))
    runtimeError("Assets: `%s` is not a folder", name);

  auto f = AssetFolder::make(".");

  f->initialize(path, ext);
  return f;
}

namespace
{ // begin namespace

inline auto
filter(const fs::path& path, const char* ext)
{
  return ext == nullptr? true : path.extension() == ext;
}

} // emd namespace

void
AssetFolder::initialize(const fs::path& path, const char* ext)
{
  auto dit = fs::directory_iterator(_path = path);

  for (auto e = fs::directory_iterator(); dit != e; ++dit)
  {
    auto s = dit->status();
    const auto& p = dit->path();

    if (fs::is_directory(s))
    {
      auto filename = p.filename().string();
      auto file = AssetFolder::make(filename.c_str(), this);

      file->initialize(p, ext);
      _folders.insert(file);
    }
    else if (fs::is_regular_file(s) && filter(p, ext))
    {
      auto filename = p.filename().string();
      _files.insert(AssetFile::make(filename.c_str(), this));
    }
  }
}

bool
AssetFolder::addFile(const char* filename)
{
  auto p = _path / filename;

  if (!fs::is_regular_file(p))
    return false;
  _files.insert(AssetFile::make(filename, this));
  return true;
}

} // end namespace cg
