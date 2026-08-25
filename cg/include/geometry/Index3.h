//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2026 Paulo Pagliosa.                        |
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
// Last revision: 24/08/2026

#ifndef __Index3_h
#define __Index3_h

#include "geometry/Index2.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// Index3: 3D index class
// ======
template <IsInt T>
struct Index<3, T>
{
  using type = Index<3, T>;
  using base_type = T;

  union
  {
    struct { T x; T y; T z; };
    struct { T i; T j; T k; };
  };

  Index() = default;

  HOST DEVICE
  constexpr Index(T i, T j, T k = 0)
  {
    set(i, j, k);
  }

  template <typename V>
  HOST DEVICE
  explicit constexpr Index(const V& v)
  {
    set(v);
  }

  HOST DEVICE
  constexpr void set(T i, T j, T k = 0)
  {
    x = i;
    y = j;
    z = k;
  }

  template <typename V>
  HOST DEVICE
  constexpr void set(const V& v)
  {
    if constexpr (std::is_integral_v<V>)
      x = y = z = T(v);
    else
      set(T(v.x), T(v.y), T(v.z));
  }

  [[nodiscard]] HOST DEVICE
  constexpr auto operator +(const Index& other) const
  {
    return Index{x + other.x, y + other.y, z + other.z};
  }

  [[nodiscard]] HOST DEVICE
  constexpr auto operator +(T i) const
  {
    return operator +(Index{i});
  }

  [[nodiscard]] HOST DEVICE
  constexpr auto operator -(const Index& other) const
  {
    return Index{x - other.x, y - other.y, z - other.z};
  }

  [[nodiscard]] HOST DEVICE
  constexpr auto operator -(T i) const
  {
    return operator -(Index{i});
  }

  [[nodiscard]] HOST DEVICE
  const auto& operator [](int i) const
  {
    return (&x)[i];
  }

  [[nodiscard]] HOST DEVICE
  auto& operator [](int i)
  {
    return (&x)[i];
  }

  [[nodiscard]] HOST DEVICE
  bool operator ==(const Index& other) const
  {
    return x == other.x && y == other.y && z == other.z;
  }

  [[nodiscard]] HOST DEVICE
  bool operator !=(const Index& other) const
  {
    return !operator ==(other);
  }

  [[nodiscard]] HOST DEVICE
  constexpr auto min() const
  {
    return math::min(math::min(x, y), z);
  }

  [[nodiscard]] HOST DEVICE
  constexpr auto max() const
  {
    return math::max(math::max(x, y), z);
  }

  HOST DEVICE
  auto& clamp(const Index& s)
  {
    x = x < 0 ? 0 : math::min(x, s.x - 1);
    y = y < 0 ? 0 : math::min(y, s.y - 1);
    z = z < 0 ? 0 : math::min(z, s.z - 1);
    return *this;
  }

  [[nodiscard]] HOST DEVICE
  constexpr auto prod() const
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
