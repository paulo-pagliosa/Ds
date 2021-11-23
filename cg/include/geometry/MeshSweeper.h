//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2019 Orthrus Group.                         |
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
// OVERVIEW: MeshSweeper.h
// ========
// Class definition for mesh sweeper.
//
// Author: Paulo Pagliosa
// Last revision: 18/11/2019

#ifndef __MeshSweeper_h
#define __MeshSweeper_h

#include "geometry/TriangleMesh.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// MeshSweeper: mesh sweeper class
// ===========
class MeshSweeper
{
public:
  static TriangleMesh* makeBox();
  static TriangleMesh* makeCone(int ns = 16);
  static TriangleMesh* makeSphere(int ns = 16);

}; // MeshSweeper

} // end namespace cg

#endif // __MeshSweeper_h
