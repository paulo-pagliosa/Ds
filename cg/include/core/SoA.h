//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019, 2026 Paulo Pagliosa.                        |
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
// Last revision: 06/10/2026

#ifndef __SoA_h
#define __SoA_h

#include "core/Globals.h"
#include <cassert>
#include <concepts>
#include <cstring>
#include <tuple>
#include <utility>

namespace cg
{ // begin namespace cg

#define ASSERT_IS_NOT_VOID(T, msg) static_assert(!std::is_void_v<T>, msg)

template <typename A, typename T>
concept IsAllocator = requires (size_t n, T* ptr)
{
  { A::template allocate<T>(n) } -> std::same_as<T*>;
  { A::template free<T>(ptr) };
};

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
  void allocate(size_t)
  {
    // do nothing
  }

  template <typename Allocator>
  void free()
  {
    // do nothing
  }

  void swap(index_t, index_t)
  {
    // do nothing
  }

}; // Arrays

template <typename index_t, typename T, typename... Args>
class Arrays<index_t, T, Args...>: private Arrays<index_t, Args...>
{
public:
  ASSERT_IS_NOT_VOID(T, "SoA: array type cannot be void");

  using Base = Arrays<index_t, Args...>;

  T* data;

  [[nodiscard]] HOST DEVICE
  const Base& base() const
  {
    return *this;
  }

  [[nodiscard]] HOST DEVICE
  Base& base()
  {
    return *this;
  }

  template <typename Allocator>
    requires IsAllocator<Allocator, T>
  void allocate(size_t count)
  {
    Base::template allocate<Allocator>(count);
    try
    {
      data = Allocator::template allocate<T>(count);
    }
    catch (...)
    {
      // Release the arrays already allocated by the base,
      // so that a failed allocation does not leak
      Base::template free<Allocator>();
      throw;
    }
  }

  template <typename Allocator>
    requires IsAllocator<Allocator, T>
  void free()
  {
    Allocator::template free<T>(data);
    // Reset data, so that a later failed allocation cannot
    // cause a double free
    data = nullptr;
    Base::template free<Allocator>();
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
  [[nodiscard]] const auto& get() const
  {
    return const_cast<SoA*>(_soa)->template get<I>(_index);
  }

  [[nodiscard]] auto tuple() const
  {
    return _soa->tuple(_index);
  }

  [[nodiscard]] auto index() const
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

  [[nodiscard]] bool operator ==(const const_iterator& other) const
  {
    return _soa == other._soa && _index == other._index;
  }

  [[nodiscard]] bool operator !=(const const_iterator& other) const
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
  [[nodiscard]] auto& get() const
  {
    return this->_soa->template get<I>(this->_index);
  }

  void set(const Args&... args) const
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

  [[nodiscard]] HOST DEVICE
  auto size() const
  {
    return _size;
  }

  template <size_t I>
  [[nodiscard]] HOST DEVICE
  auto data()
  {
    using dt = soa::Data<I, index_t, soa::Arrays<index_t, Args...>>;
    return ((typename dt::array_type&)_arrays).data;
  }

  template <size_t I>
  [[nodiscard]] HOST DEVICE
  const auto* data() const
  {
    return const_cast<SoABase*>(this)->template data<I>();
  }

  template <size_t I>
  [[nodiscard]] HOST DEVICE
  auto& get(index_t i)
  {
#ifndef __CUDA_ARCH__
    assert(i >= 0 && i < _size);
#endif // __CUDA_ARCH__
    return this->template data<I>()[i];
  }

  template <size_t I>
  [[nodiscard]] HOST DEVICE
  const auto& get(index_t i) const
  {
    return const_cast<SoABase*>(this)->template get<I>(i);
  }

  [[nodiscard]] tuple_type tuple(index_t i) const
  {
    assert(i >= 0 && i < _size);
    return fields(i, Indices{});
  }

  void set(index_t i, const Args&... args)
  {
    assert(i >= 0 && i < _size);
    setFields(i, std::forward_as_tuple(args...), Indices{});
  }

  void setTuple(index_t i, const tuple_type& t)
  {
    assert(i >= 0 && i < _size);
    setFields(i, t, Indices{});
  }

  void swap(index_t i, index_t j)
  {
    assert(i >= 0 && i < _size && j >= 0 && j < _size);
    _arrays.swap(i, j);
  }

protected:
  using Indices = std::index_sequence_for<Args...>;

  soa::Arrays<index_t, Args...> _arrays;
  index_t _size;

private:
  template <size_t... I>
  tuple_type fields(index_t i, std::index_sequence<I...>) const
  {
    return tuple_type{this->template get<I>(i)...};
  }
 
  template <typename Tuple, size_t... I>
  void setFields(index_t i, const Tuple& t, std::index_sequence<I...>)
  {
    ((this->template data<I>()[i] = std::get<I>(t)), ...);
  }
 
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
    free();
  }

  SoA()
  {
    this->_arrays = {};
    this->_size = 0;
  }

  SoA(index_t size):
    SoA{}
  {
    assert(size >= 0);
    if (size > 0)
      this->_arrays.template allocate<Allocator>((size_t)size);
    this->_size = size;
  }

  SoA(const SoA&) = delete;
  SoA& operator =(const type&) = delete;

  SoA(SoA&& other) noexcept
  {
    this->_arrays = std::exchange(other._arrays, {});
    this->_size = std::exchange(other._size, 0);
  }

  SoA& operator =(SoA&& other) noexcept
  {
    if (this != &other)
    {
      free();
      this->_arrays = std::exchange(other._arrays, {});
      this->_size = std::exchange(other._size, 0);
    }
    return *this;
  }

  bool reallocate(index_t size)
  {
    assert(size >= 0);
    if (size == this->_size)
      return false;
    free();
    this->_size = 0;
 
    soa::Arrays<index_t, Args...> arrays{};
 
    if (size > 0)
    {
      arrays.template allocate<Allocator>((size_t)size);
      this->_size = size;
    }
    this->_arrays = arrays;
    return true;
  }

  auto& copy(const SoA& other)
  {
    if (this != &other)
    {
      reallocate(other._size);
      copyArrays(other, typename Base::Indices{});
    }
    return *this;
  }

  template <size_t I>
    requires (I < sizeof...(Args))
  void copyArray(const SoA& other)
  {
    assert(this != &other && this->_size == other._size);
    copyArrayData<I>(other);
  }

  [[nodiscard]] auto cbegin() const
  {
    return const_iterator{this, 0};
  }

  [[nodiscard]] auto cend() const
  {
    return const_iterator{this, this->_size};
  }

  [[nodiscard]] auto begin() const
  {
    return cbegin();
  }

  [[nodiscard]] auto end() const
  {
    return cend();
  }

  [[nodiscard]] auto begin()
  {
    return iterator{this, 0};
  }

  [[nodiscard]] auto end()
  {
    return iterator{this, this->_size};
  }

private:
  void free() noexcept
  {
    this->_arrays.template free<Allocator>();
  }

  template <size_t I>
  void copyArrayData(const SoA& other)
  {
    if (this->_size <= 0)
      return;
 
    auto dst = this->template data<I>();
    auto src = other.template data<I>();
 
    using D = std::remove_cvref_t<decltype(*dst)>;
 
    if constexpr (std::is_trivially_copyable_v<D>)
      std::memcpy(dst, src, (size_t)this->_size * sizeof(D));
    else
      for (index_t i = 0; i < this->_size; ++i)
        dst[i] = src[i];
  }
 
  template <size_t... I>
  void copyArrays(const SoA& other, std::index_sequence<I...>)
  {
    (copyArrayData<I>(other), ...);
  }

}; // SoA

namespace soa
{ // begin namespace soa

template <size_t I, typename SoA>
[[nodiscard]] HOST DEVICE
inline const auto&
get(const SoA& soa, typename SoA::index_type i)
{
  return soa.template get<I>(i);
}

template <size_t I, typename SoA>
[[nodiscard]] HOST DEVICE
inline auto&
get(SoA& soa, typename SoA::index_type i)
{
  return soa.template get<I>(i);
}

template <typename SoA>
[[nodiscard]] inline auto
tuple(const SoA& soa, typename SoA::index_type i)
{
  return soa.tuple(i);
}

template <typename index_t, typename... Args>
inline void
set(SoABase<index_t, Args...>& soa, index_t i, const Args&... args)
{
  soa.set(i, args...);
}

template <typename SoA>
inline void
setTuple(SoA& soa,
  typename SoA::index_type i,
  const typename SoA::tuple_type& t)
{
  soa.setTuple(i, t);
}

template <typename SoA>
inline auto
copy(SoA& dst, const SoA& src)
{
  return dst.copy(src);
}

} // end namespace soa

} // end namespace cg

#endif // __SoA_h
