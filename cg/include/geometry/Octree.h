//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2021 Paulo Pagliosa.                        |
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
// OVERVIEW: Octree.h
// ========
// Class definition for generic octree.
//
// Author: Paulo Pagliosa
// Last revision: 06/12/2021

//         Octree children configuration
//           ________________________
//          /           /           /|
//         /    3      /    7      / |
//        /___________/___________/  |
//       /           /           /| 7|
//      /     2     /     6     / | /|           z
//     /___________/___________/  |/ |        y /
//     |           |          | 6 /  |        |/
//     |     2     |     6    |  /| 5|        |---x
//     |           |          | / | /
//     |___________|__________|/  |/
//     |           |          |   /
//     |     0     |     4    | 4/
//     |           |          | /
//     |___________|__________|/

#ifndef __Octree_h
#define __Octree_h

#include "geometry/Index3.h"
#include "geometry/TreeBase.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// OctreeKey: octree key class
// =========
template <>
class TreeKey<3> final: public Index<3>
{
public:
  using Index<3>::Index;

  auto& pushChild(int index)
  {
    x = (x << 1) | (!!(index & (1 << 2)));
    y = (y << 1) | (!!(index & (1 << 1)));
    z = (z << 1) | (!!(index & (1 << 0)));
    return *this;
  }

  auto& popChild()
  {
    x >>= 1;
    y >>= 1;
    z >>= 1;
    return *this;
  }

  auto& popChildren(int n)
  {
    x >>= n;
    y >>= n;
    z >>= n;
    return *this;
  }

  int childIndex(uint64_t mask) const
  {
    return ((!!(x & mask)) << 2) | ((!!(y & mask)) << 1) | (!!(z & mask));
  }

  auto childKey(int i) const
  {
    return TreeKey<3>{x, y, z}.pushChild(i);
  }

}; // TreeKey

using OctreeKey = TreeKey<3>;


/////////////////////////////////////////////////////////////////////
//
// Octree: generic octree class
// ======
template <>
class TreeNeighborInfo<3>
{
public:
  static constexpr auto S = 4;

  static const int* neighborCode(int direction)
  {
    static const int nt[][8]
    {
      {4, 0, 5, 1, 6, 2, 7, 3}, // LEFT
      {0, 4, 1, 5, 2, 6, 3, 7}, // RIGHT
      {2, 0, 3, 1, 6, 4, 7, 5}, // DOWN
      {0, 2, 1, 3, 4, 6, 5, 7}, // UP
      {1, 0, 3, 2, 5, 4, 7, 6}, // FRONT
      {0, 1, 2, 3, 4, 5, 6, 7}  // BACK
    };

    return nt[direction];
  }

  static const int* neighborChildCode(int direction)
  {
    static const int nct[][4]
    {
      {4, 5, 6, 7}, // LEFT
      {0, 1, 2, 3}, // RIGHT
      {2, 3, 6, 7}, // DOWN
      {0, 1, 4, 5}, // UP
      {1, 3, 5, 7}, // FRONT
      {0, 2, 4, 6}  // BACK
    };

    return nct[direction];
  }

}; // TreeNeighborInfo

template <typename real, typename LT, typename BT = void>
using Octree = RegionTree<3, real, LT, BT>;

} // namespace cg

#endif // __Octree_h
