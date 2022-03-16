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
// OVERVIEW: TriCellMeshReader.cpp
// ========
// Source file for vis triangle mesh reader.
//
// Author: Paulo Pagliosa
// Last revision: 15/03/2022

#include "utils/MeshReader.h"
#include "TriCellMeshReader.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// TriCellMeshReader implementation
// =================
void
TriCellMeshReader::setPath(const std::string& filename)
{
  auto path = fs::absolute(filename);

  if (!fs::is_regular_file(path) || !fs::exists(path))
  {
    constexpr auto maxLen = 1024;
    char msg[maxLen];

    snprintf(msg,
      maxLen,
      "Could not find mesh file: '%s'",
      filename.c_str());
    throw std::runtime_error(msg);
  }
  _path = path;
}

void
TriCellMeshReader::execute()
{
  setOutput(TriCellMesh::New(*MeshReader::readOBJ(_path.string().c_str())));
#ifdef _DEBUG
  puts("MESH READER EXECUTED");
#endif // _DEBUG
}

} // end namespace cg::vis
