//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: Index3.h
// ========
// Class definition for 3D index.
//
// Author: Paulo Pagliosa
// Last revision: 13/12/2022

#ifndef __Index3_h
#define __Index3_h

#include "geometry/Index2.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Index3: 3D index class
// ======
template <typename T>
struct Index<3, T>
{
  ASSERT_INT(T, "Index3: integral type expected");

  using type = Index<3, T>;
  using base_type = T;

  base_type x;
  base_type y;
  base_type z;

  HOST DEVICE
  Index()
  {
    // do nothing
  }

  HOST DEVICE
  Index(base_type i, base_type j, base_type k = 0)
  {
    set(i, j, k);
  }

  HOST DEVICE
  explicit Index(base_type i)
  {
    set(i, i, i);
  }

  template <typename V>
  HOST DEVICE
  explicit Index(const V& v)
  {
    set(v);
  }

  template <typename V>
  HOST DEVICE
  void set(const V& v)
  {
    if constexpr (std::is_integral_v<V>)
      x = y = z = base_type(v);
    else
      set(base_type(v.x), base_type(v.y), base_type(v.z));
  }

  HOST DEVICE
  void set(base_type i, base_type j, base_type k = 0)
  {
    x = i;
    y = j;
    z = k;
  }

  HOST DEVICE
  auto& operator =(base_type i)
  {
    set(i, i, i);
    return *this;
  }

  HOST DEVICE
  auto operator +(const type& other) const
  {
    return type{x + other.x, y + other.y, z + other.z};
  }

  HOST DEVICE
  auto operator +(base_type i) const
  {
    return operator +(type{i});
  }

  HOST DEVICE
  auto operator -(const type& other) const
  {
    return type{x - other.x, y - other.y, z - other.z};
  }

  HOST DEVICE
  auto operator -(base_type i) const
  {
    return operator -(type{i});
  }

  HOST DEVICE
  const auto& operator [](int i) const
  {
    return (&x)[i];
  }

  HOST DEVICE
  auto& operator [](int i)
  {
    return (&x)[i];
  }

  HOST DEVICE
  bool operator ==(const type& other) const
  {
    return x == other.x && y == other.y && z == other.z;
  }

  HOST DEVICE
  bool operator !=(const type& other) const
  {
    return !operator ==(other);
  }

  HOST DEVICE
  auto min() const
  {
    return math::min(math::min(x, y), z);
  }

  HOST DEVICE
  auto max() const
  {
    return math::max(math::max(x, y), z);
  }

  HOST DEVICE
  auto& clamp(const type& s)
  {
    x = x < 0 ? 0 : math::min(x, s.x - 1);
    y = y < 0 ? 0 : math::min(y, s.y - 1);
    z = z < 0 ? 0 : math::min(z, s.z - 1);
    return *this;
  }

  HOST DEVICE
  auto prod() const
  {
    return x * y * z;
  }

  void print(const char* s, std::ostream& f = std::cout) const
  {
    f << s << '(' << x << ',' << y << ',' << z << ")\n";
  }

}; // Index3

template <typename T = int64_t> using Index3 = Index<3, T>;

} // end namespace cg

#endif // __Index3_h
