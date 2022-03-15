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
// OVERVIEW: PointSet.h
// ========
// Class definition for vis point set.
//
// Author: Paulo Pagliosa
// Last revision: 14/03/2022

#ifndef __VisPointSet_h
#define __VisPointSet_h

#include "DataSet.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// PoinSet: vis point set class
// =======
class PointSet: public DataSet, public DataArray<vec3f>
{
public:
  using Points = DataArray<vec3f>;

  static Reference<PointSet> New(uint32_t size = 0)
  {
    return new PointSet{size};
  }

  auto vertexCount() const
  {
    return size();
  }

  const auto& vertex(int i) const
  {
    return get(i);
  }

  auto bounds() const
  {
    return _bounds;
  }

  void add(const vec3f& p)
  {
    Points::add(p);
    _bounds.inflate(p);
  }

  void set(int i, const vec3f& p)
  {
    Points::set(i, p);
    _bounds.inflate(p);
  }

private:
  Bounds3f _bounds;

  PointSet(uint32_t size):
    Points{size}
  {
    // do nothing
  }

}; // PointSet

} // end namespace cg::vis

#endif // __PointSet_h


