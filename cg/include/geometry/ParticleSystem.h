//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: ParticleSystem.h
// ========
// Class definition for particle system.
//
// Author: Paulo Pagliosa
// Last revision: 11/09/2022

#ifndef __ParticleSystem_h
#define __ParticleSystem_h

#include "core/SoA.h"
#include "math/Matrix4x4.h"

namespace cg
{ // begin namespace cg


//////////////////////////////////////////////////////////
//
// ParticleSystem: particle system class
// ==============
template <int D, typename real, typename Allocator, typename... Args>
class ParticleSystem
{
public:
  using vec_type = Vector<real, D>;
  using Data = SoA<Allocator, vec_type, Args...>;
  using type = ParticleSystem<D, real, Allocator, Args...>;

  ParticleSystem() = default;

  ParticleSystem(size_t capacity):
    _data{capacity}
  {
    // do nothing
  }

  auto capacity() const
  {
    return _data.size();
  }

  auto size() const
  {
    return _size;
  }

  void realloc(size_t capacity)
  {
    if (_data.realloc(capacity))
      _size = 0;
  }

  void clear()
  {
    _size = 0;
  }

  bool add(const vec_type& p, const Args&... args)
  {
    if (_size == capacity())
      return false;
    set(_size++, p, args...);
    return true;
  }

  bool remove(size_t i)
  {
    if (i >= _size)
      return false;
    _data.swap(i, --_size);
    return true;
  }

  template <size_t I>
  constexpr const auto& get(size_t i) const
  {
    assert(i < _size);
    return _data.template get<I>(i);
  }

  template <size_t I>
  constexpr auto& get(size_t i)
  {
    assert(i < _size);
    return _data.template get<I>(i);
  }

  void set(size_t i, const vec_type& p, const Args&... args)
  {
    assert(i < _size);
    _data.set(i, p, args...);
  }

  constexpr const auto& position(size_t i) const
  {
    return this->template get<0>(i);
  }

  constexpr auto& position(size_t i)
  {
    return this->template get<0>(i);
  }

  constexpr void setPosition(size_t i, const vec_type& p)
  {
    position(i) = p;
  }

  const auto& operator [](size_t i) const
  {
    return position(i);
  }

  auto& operator [](size_t i)
  {
    return position(i);
  }

  auto cbegin() const
  {
    return _data.cbegin();
  }

  auto cend() const
  {
    return typename Data::const_iterator{&_data, _size};
  }

  auto begin()
  {
    return _data.begin();
  }

  auto end()
  {
    return typename Data::iterator{&_data, _size};
  }

protected:
  Data _data;
  size_t _size{};

}; // ParticleSystem


template <typename real, typename Allocator, typename... Args>
using ParticleSystem2 = ParticleSystem<2, real, Allocator, Args...>;

template <typename real, typename Allocator, typename... Args>
using ParticleSystem3 = ParticleSystem<3, real, Allocator, Args...>;

} // end namespace cg

#endif // __ParticleSystem_h
