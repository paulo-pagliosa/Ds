//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2023 Paulo Pagliosa.                              |
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
// OVERVIEW: MeshWriter.h
// ========
// Class definition for mesh writer.
//
// Author: Paulo Pagliosa
// Last revision: 29/08/2023

#ifndef __MeshWriter_h
#define __MeshWriter_h

#include "geometry/TriangleMesh.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// MeshWriter: mesh writer class
// ==========
class MeshWriter
{
public:
  static bool writeOBJ(const TriangleMesh& mesh, const char* filename);

}; // MeshWriter

} // end namespace cg

#endif // __MeshWriter_h
