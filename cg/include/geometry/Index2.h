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
// OVERVIEW: Index2.h
// ========
// Class definition for 2D index.
//
// Author: Paulo Pagliosa
// Last revision: 13/12/2022

#ifndef __Index2_h
#define __Index2_h

#include "math/Real.h"
#include <iostream>

namespace cg
{ // begin namespace cg

template <typename T>
inline constexpr bool
isInt()
{
  return std::is_integral_v<T>;
}

#define ASSERT_INT(T, msg) static_assert(isInt<T>(), msg)

template <int D, typename T = int64_t> struct Index;


/////////////////////////////////////////////////////////////////////
//
// Index2: 2D index class
// ======
template <typename T>
struct Index<2, T>
{
  ASSERT_INT(T, "Index2: integral type expected");

  using type = Index<2, T>;
  using base_type = T;

  union
  {
    struct { base_type x, y; };
    struct { base_type u, v; };
  };

  HOST DEVICE
  Index()
  {
    // do nothing
  }

  HOST DEVICE
  Index(base_type i, base_type j)
  {
    set(i, j);
  }

  template <typename V>
  HOST DEVICE
  explicit Index(const V& v)
  {
    set(v);
  }

  HOST DEVICE
  void set(base_type i, base_type j)
  {
    x = i;
    y = j;
  }

  template <typename V>
  HOST DEVICE
  void set(const V& v)
  {
    if constexpr (std::is_integral_v<V>)
      x = y = base_type(v);
    else
      set(base_type(v.x), base_type(v.y));
  }

  HOST DEVICE
  auto& operator =(base_type i)
  {
    set(i, i);
    return *this;
  }

  HOST DEVICE
  auto operator +(const type& other) const
  {
    return type{x + other.x, y + other.y};
  }

  HOST DEVICE
  auto operator +(base_type i) const
  {
    return operator +(type{i});
  }

  HOST DEVICE
  auto operator -(const type& other) const
  {
    return type{x - other.x, y - other.y};
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
    return x == other.x && y == other.y;
  }

  HOST DEVICE
  bool operator !=(const type& other) const
  {
    return !operator ==(other);
  }

  HOST DEVICE
  auto min() const
  {
    return math::min(x, y);
  }

  HOST DEVICE
  auto max() const
  {
    return math::max(x, y);
  }

  HOST DEVICE
  auto& clamp(const type& s)
  {
    x = x < 0 ? 0 : math::min(x, s.x - 1);
    y = y < 0 ? 0 : math::min(y, s.y - 1);
    return *this;
  }

  HOST DEVICE
  auto prod() const
  {
    return x * y;
  }

  void print(const char* s, std::ostream& f = std::cout) const
  {
    f << s << '(' << x << ',' << y << ")\n";
  }

}; // Index2

template <typename T = int64_t> using Index2 = Index<2, T>;

} // end namespace cg

#endif // __Index2_h
