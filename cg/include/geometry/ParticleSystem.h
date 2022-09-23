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
// Last revision: 19/09/2022

#ifndef __ParticleSystem_h
#define __ParticleSystem_h

#include "core/SoA.h"
#include "geometry/IndexList.h"
#include "math/Matrix4x4.h"

namespace cg
{ // begin namespace cg


//////////////////////////////////////////////////////////
//
// ParticleSystem: particle system class
// ==============
template <class Allocator, class index_t, class Vector, class... Args>
class ParticleSystem
{
public:
  using PointId = index_t;
  using Data = SoA<Allocator, index_t, Vector, Args...>;
  using type = ParticleSystem<Allocator, index_t, Vector, Args...>;

  ParticleSystem() = default;

  ParticleSystem(index_t capacity):
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

  void reallocate(index_t capacity)
  {
    if (_data.reallocate(capacity))
      clear();
  }

  void clear()
  {
    _size = 0;
  }

  bool add(const Vector& p, const Args&... args)
  {
    PointId i;

    if (_freeList.size() > 0)
    {
      auto f = _freeList.begin();

      i = *f;
      _freeList.remove(f);
    }
    else if (_size < capacity())
      i = _size++;
    else
      return false;
    set(i, p, args...);
    return true;
  }

  bool remove(PointId i)
  {
    return i > 0 && i <= _size ? _freeList.add(i) : false;
  }

  template <size_t I>
  const auto& get(PointId i) const
  {
    assert(i >= 0 && i < _size);
    return _data.template get<I>(i);
  }

  template <size_t I>
  auto& get(PointId i)
  {
    assert(i >= 0 && i < _size);
    return _data.template get<I>(i);
  }

  void set(PointId i, const Vector& p, const Args&... args)
  {
    assert(i >= 0 && i < _size);
    _data.set(i, p, args...);
  }

  const auto& position(PointId i) const
  {
    return this->template get<0>(i);
  }

  auto& position(PointId i)
  {
    return this->template get<0>(i);
  }

  void setPosition(PointId i, const Vector& p)
  {
    position(i) = p;
  }

  const auto& operator [](PointId i) const
  {
    return position(i);
  }

  auto& operator [](PointId i)
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
  PointId _size{};
  IndexList<PointId> _freeList;
  // TODO: bitset of inactive point flags

}; // ParticleSystem

} // end namespace cg

#endif // __ParticleSystem_h
