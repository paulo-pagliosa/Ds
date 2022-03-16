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
// OVERVIEW: TriCellMeshReader.h
// ========
// Class definition for vis triangle mesh reader.
//
// Author: Paulo Pagliosa
// Last revision: 15/03/2022

#ifndef __TriCellMeshReader_h
#define __TriCellMeshReader_h

#include "Source.h"
#include "TriCellMesh.h"
#include <filesystem>

namespace cg
{ // begin namespace cg

namespace fs = std::filesystem;

namespace vis
{ // begin namespace vis


/////////////////////////////////////////////////////////////////////
//
// TriCellMeshReader: triangle mesh reader class
// =================
class TriCellMeshReader final: public Source<TriCellMesh>
{
public:
  using Base = Source<TriCellMesh>;

  static Reference<TriCellMeshReader> New()
  {
    return new TriCellMeshReader;
  }

  const auto& path() const
  {
    return _path;
  }

  void setPath(const std::string& filename);

  void execute() override;

private:
  fs::path _path;

}; // TriCellMeshReader

} // end namespace vis

} // end namespace cg

#endif // __TriCellMeshReader_h
