//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2025 Paulo Pagliosa.                        |
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
// OVERVIEW: SoA.h
// ========
// Class definition for structure of arrays.
//
// Author: Paulo Pagliosa
// Last revision: 27/05/2025

#ifndef __SoA_h
#define __SoA_h

#include "core/Array.h"
#include "core/Globals.h"
#include <cassert>
#include <tuple>

namespace cg
{ // begin namespace cg

template <typename index_t, typename... Args> class SoABase;

namespace soa
{ // begin namespace soa

template <typename index_t, typename... Args> class Arrays;
template <size_t I, typename index_t, typename Arrays> struct Data;

template <typename>
constexpr bool as_false = false;

template <size_t I, typename index_t>
struct Data<I, index_t, Arrays<index_t>>
{
  // Enforce bounds checking
  static_assert(as_false<std::integral_constant<index_t, I>>,
    "SoA: array index out of bounds");

}; // Data

template <typename index_t, typename T, typename... Args>
struct Data<0, index_t, Arrays<index_t, T, Args...>>
{
  // Select first array
  using type = T*;
  using array_type = Arrays<index_t, T, Args...>;

}; // Data

template<size_t I, typename index_t, typename T, typename... Args>
struct Data<I, index_t, Arrays<index_t, T, Args...>>:
  public Data<I - 1, index_t, Arrays<index_t, Args...>>
{
  // empty

}; // Data

template <typename index_t, typename... Args>
class Arrays
{
public:
  template <typename Allocator>
  HOST DEVICE
  void allocate(index_t n)
  {
    // do nothing
  }

  template <typename Allocator>
  HOST DEVICE
  void free()
  {
    // do nothing
  }

  HOST DEVICE
  void get(index_t i, std::tuple<Args...>& t) const
  {
    // do nothing
  }

  HOST DEVICE
  void set(index_t i, const std::tuple<Args...>& t)
  {
    // do nothing
  }

  HOST DEVICE
  void swap(index_t i, index_t j)
  {
    // do nothing
  }

}; // Arrays

template <typename index_t, typename T, typename... Args>
class Arrays<index_t, T, Args...>: private Arrays<index_t, Args...>
{
public:
  static_assert(!std::is_void_v<T>, "SoA: array type cannot be void");

  using Base = Arrays<index_t, Args...>;

  T* data;

  HOST DEVICE
  const Base& base() const
  {
    return *this;
  }

  HOST DEVICE
  Base& base()
  {
    return *this;
  }

  template <typename Allocator>
  void allocate(index_t count)
  {
    Base::template allocate<Allocator>(count);
    data = Allocator::template allocate<T>(count);
  }

  template <typename Allocator>
  void free()
  {
    Allocator::template free<T>(data);
    Base::template free<Allocator>();
  }

  void get(index_t i, std::tuple<T, Args...>& t) const
  {
    std::get<0>(t) = data[i];
    Base::get(i, (std::tuple<Args...>&)t);
  }

  void set(index_t i, const std::tuple<T, Args...>& t)
  {
    Base::set(i, (std::tuple<Args...>&)t);
    data[i] = std::get<0>(t);
  }

  HOST DEVICE
  void swap(index_t i, index_t j)
  {
    std::swap(data[i], data[j]);
    Base::swap(i, j);
  }

}; // Arrays

} // end namespace soa


/////////////////////////////////////////////////////////////////////
//
// SoAConstIterator: SoA const iterator class
// ================
template <typename index_t, typename... Args>
class SoAConstIterator
{
public:
  using const_iterator = SoAConstIterator<index_t, Args...>;
  using SoA = SoABase<index_t, Args...>;

  SoAConstIterator() = default;

  SoAConstIterator(const SoA* soa, index_t index):
    _soa{const_cast<SoA*>(soa)},
    _index{index}
  {
    // do nothing
  }

  template <size_t I>
  const auto& get() const
  {
    return const_cast<SoA*>(_soa)->template get<I>(_index);
  }

  auto tuple() const
  {
    return _soa->tuple(_index);
  }

  auto index() const
  {
    return _index;
  }

  const_iterator& operator ++()
  {
    ++_index;
    return *this;
  }

  const_iterator operator ++(int)
  {
    const_iterator temp{*this};

    _index++;
    return temp;
  }

  const_iterator& operator --()
  {
    --_index;
    return *this;
  }

  const_iterator operator --(int)
  {
    const_iterator temp{*this};

    _index--;
    return temp;
  }

  bool operator ==(const const_iterator& other) const
  {
    return _soa == other._soa && _index == other._index;
  }

  bool operator !=(const const_iterator& other) const
  {
    return !operator ==(other);
  }

protected:
  SoA* _soa{};
  index_t _index{};

}; // SoAConstIterator


/////////////////////////////////////////////////////////////////////
//
// SoAIterator: SoA iterator class
// ===========
template <typename index_t, typename... Args>
class SoAIterator: public SoAConstIterator<index_t, Args...>
{
public:
  using const_iterator = SoAConstIterator<index_t, Args...>;
  using iterator = SoAIterator<index_t,Args...>;
  using SoA = SoABase<index_t, Args...>;

  SoAIterator() = default;

  SoAIterator(SoA* soa, index_t index):
    const_iterator{const_cast<SoA*>(soa), index}
  {
    // do nothing
  }

  template <size_t I>
  auto& get() const
  {
    return this->_soa->template get<I>(this->_index);
  }

  HOST DEVICE
  void set(const Args&... args)
  {
    return this->_soa->set(this->_index, args...);
  }

  void setTuple(const typename SoA::tuple_type& t) const
  {
    return this->_soa->setTuple(this->_index, t);
  }

  iterator& operator ++()
  {
    ++*((const_iterator*)this);
    return *this;
  }

  iterator operator ++(int)
  {
    iterator temp{*this};

    ++*this;
    return temp;
  }

  iterator& operator --()
  {
    --*((const_iterator*)this);
    return *this;
  }

  iterator operator --(int)
  {
    iterator temp{*this};

    --*this;
    return temp;
  }

}; // SoAIterator


/////////////////////////////////////////////////////////////////////
//
// SoABase: structure of arrays base class
// =======
template <typename index_t, typename... Args>
class SoABase
{
public:
  static constexpr auto arrayCount = sizeof...(Args);

  using index_type = index_t;
  using tuple_type = std::tuple<Args...>;

  HOST DEVICE
  auto size() const
  {
    return _size;
  }

  template <size_t I>
  HOST DEVICE
  const auto data() const
  {
    using dt = soa::Data<I, index_t, soa::Arrays<index_t, Args...>>;
    return ((typename dt::array_type&)_arrays).data;
  }

  template <size_t I>
  HOST DEVICE
  auto data()
  {
    using dt = soa::Data<I, index_t, soa::Arrays<index_t, Args...>>;
    return ((typename dt::array_type&)_arrays).data;
  }

  template <size_t I>
  HOST DEVICE
  const auto& get(index_t i) const
  {
#ifndef __NVCC__
    assert(i < _size);
#endif // __NVCC__
    return this->template data<I>()[i];
  }

  template <size_t I>
  HOST DEVICE
  auto& get(index_t i)
  {
#ifndef __NVCC__
    assert(i < _size);
#endif // __NVCC__
    return this->template data<I>()[i];
  }

  void set(index_t i, const Args&... args)
  {
    setTuple(i, tuple_type(args...));
  }

  tuple_type tuple(index_t i) const
  {
    tuple_type t;

    _arrays.get(i, t);
    return t;
  }

  void setTuple(index_t i, const tuple_type& t)
  {
#ifndef __NVCC__
    assert(i < _size);
#endif // __NVCC__
    _arrays.set(i, t);
  }

  HOST DEVICE
  void swap(index_t i, index_t j)
  {
#ifndef __NVCC__
    assert(i < _size && j < _size);
#endif // __NVCC__
    _arrays.swap(i, j);
  }

protected:
  soa::Arrays<index_t, Args...> _arrays;
  index_t _size;

}; // SoABase

   
/////////////////////////////////////////////////////////////////////
//
// SoA: structure of arrays class
// ===
template <typename Allocator, typename index_t, typename... Args>
class SoA: public SoABase<index_t, Args...>
{
public:
  using Base = SoABase<index_t, Args...>;
  using type = SoA<Allocator, index_t, Args...>;
  using const_iterator = SoAConstIterator<index_t, Args...>;
  using iterator = SoAIterator<index_t, Args...>;

  ~SoA()
  {
    if (this->_size != 0)
      this->_arrays.template free<Allocator>();
  }

  SoA()
  {
    this->_size = 0;
  }

  SoA(index_t size)
  {
    if ((this->_size = size) != 0)
      this->_arrays.template allocate<Allocator>(size);
  }

  SoA(const type&) = delete;
  type& operator =(const type&) = delete;

  SoA(type&& other) noexcept
  {
    this->_size = other._size;
    this->_arrays = other._arrays;
    other._size = 0;
  }

  type& operator =(type&& other) noexcept
  {
    if (this != &other)
    {
      this->~SoA();
      this->_size = other._size;
      this->_arrays = other._arrays;
      other._size = 0;
    }
    return *this;
  }

  bool reallocate(index_t size)
  {
    if (size == this->_size)
      return false;
    this->~SoA();
    if ((this->_size = size) != 0)
      this->_arrays.template allocate<Allocator>(size);
    return true;
  }

  auto cbegin() const
  {
    return const_iterator{this, 0};
  }

  auto cend() const
  {
    return const_iterator{this, this->_size};
  }

  auto begin() const
  {
    return cbegin();
  }

  auto end() const
  {
    return cend();
  }

  auto begin()
  {
    return iterator{this, 0};
  }

  auto end()
  {
    return iterator{this, this->_size};
  }

}; // SoA

namespace soa
{ // begin namespace soa

template <size_t I, typename SoA>
HOST DEVICE
inline const auto&
get(const SoA& soa, typename SoA::index_type i)
{
  return soa.template get<I>(i);
}

template <size_t I, typename SoA>
HOST DEVICE
inline auto&
get(SoA& soa, typename SoA::index_type i)
{
  return soa.template get<I>(i);
}

template <typename SoA>
HOST DEVICE
inline auto
tuple(const SoA& soa, typename SoA::index_type i)
{
  return soa.tuple(i);
}

template <typename index_t, typename... Args>
HOST DEVICE
inline void
set(SoABase<index_t, Args...>& soa, index_t i, const Args&... args)
{
  return soa.set(i, args...);
}

template <typename SoA>
HOST DEVICE
inline auto
setTuple(SoA& soa, typename SoA::index_type i, typename SoA::tuple_type& t)
{
  return soa.setTuple(i, t);
}

} // end namespace soa

} // end namespace cg

#endif // __SoA_h
