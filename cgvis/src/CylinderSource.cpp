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
// OVERVIEW: CylinderSource.h
// ========
// Source file for vis cylinder source.
//
// Author: Paulo Pagliosa
// Last revision: 11/03/2022

#include "geometry/MeshSweeper.h"
#include "CylinderSource.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// CylinderSource implementation
// ==============
void
CylinderSource::execute()
{
  setOutput(TriCellMesh::New(*MeshSweeper::makeCylinder(_resolution)));
#ifdef _DEBUG
  puts("CYLINDER SOURCE EXECUTED");
#endif // _DEBUG
}

} // end namespace cg::vis
