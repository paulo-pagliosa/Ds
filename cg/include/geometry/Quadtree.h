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
// OVERVIEW: Quadtree.h
// ========
// Class definition for generic quadtree.
//
// Author: Paulo Pagliosa
// Last revision: 06/12/2021

// Quadtree children configuration
//     ______________________
//    |           |          |
//    |     1     |     3    |
//    |           |          |      y
//    |___________|__________|      |
//    |           |          |      |---x
//    |     0     |     2    |
//    |           |          |
//    |___________|__________|

#ifndef __Quadtree_h
#define __Quadtree_h

#include "geometry/Index2.h"
#include "geometry/TreeBase.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// QuadtreeKey: quadtree key class
// ===========
template <>
class TreeKey<2> final: public Index<2>
{
public:
  using Index<2>::Index;

  auto& pushChild(int index)
  {
    x = (x << 1) | (!!(index & (1 << 1)));
    y = (y << 1) | (!!(index & (1 << 0)));
    return *this;
  }

  auto& popChild()
  {
    x >>= 1;
    y >>= 1;
    return *this;
  }

  auto& popChildren(int n)
  {
    x >>= n;
    y >>= n;
    return *this;
  }

  int childIndex(uint64_t mask) const
  {
    return ((!!(x & mask)) << 1) | ((!!(y & mask)));
  }

  auto childKey(int i) const
  {
    return TreeKey<2>{x, y}.pushChild(i);
  }

}; // TreeKey

using QuadtreeKey = TreeKey<2>;


/////////////////////////////////////////////////////////////////////
//
// Quadtree: generic quadtree class
// ========
template <>
class TreeNeighborInfo<2>
{
public:
  static constexpr auto S = 2;

  static const int* neighborCode(int direction)
  {
    static const int nt[][4]
    {
      {2, 0, 3, 1}, // LEFT
      {0, 2, 1, 3}, // RIGHT
      {1, 0, 3, 2}, // DOWN
      {0, 1, 2, 3}  // UP
    };

    return nt[direction];
  }

  static const int* neighborChildCode(int direction)
  {
    static const int nct[][2]
    {
      {2, 3}, // LEFT
      {0, 1}, // RIGHT
      {1, 3}, // DOWN
      {0, 2}  // UP
    };

    return nct[direction];
  }

}; // TreeNeighborInfo

template <typename real, typename LT, typename BT = void>
using Quadtree = RegionTree<2, real, LT, BT>;

} // namespace cg

#endif // __Quadtree_h
