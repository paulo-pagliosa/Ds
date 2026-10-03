//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2021, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: Array.h
// ========
// Class for generic array.
//
// Author: Paulo Pagliosa
// Last revision: 03/10/2026

#ifndef __Array_h
#define __Array_h

#include <algorithm>
#include <concepts>
#include <cstring>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace cg
{ // begin namespace cg

template <typename I>
concept IsArrayIndex = std::integral<I> && !std::same_as<I, bool>;


/////////////////////////////////////////////////////////////////////
//
// ArrayIterator: array iterator class
// =============
template <typename Array>
class ArrayIterator
{
public:
  using value_type = Array::value_type;
  using iterator = ArrayIterator<Array>;
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using pointer = const value_type*;
  using reference = const value_type&;

  ArrayIterator() = default;

  ArrayIterator(const Array* array, size_t index):
    _array{array},
    _index{index}
  {
    // do nothing
  }

  [[nodiscard]] bool operator ==(const iterator& other) const
  {
    return _index == other._index && _array == other._array;
  }

  [[nodiscard]] bool operator !=(const iterator& other) const
  {
    return !operator ==(other);
  }

  auto& operator ++()
  {
#ifdef _DEBUG
    if (!_array || _index >= _array->size())
      throw std::logic_error{"Array iterator not incrementable"};
#endif // _DEBUG
    ++_index;
    return *this;
  }

  auto operator ++(int)
  {
    auto temp = *this;

    ++*this;
    return temp;
  }

  auto& operator --()
  {
#ifdef _DEBUG
    if (!_array || _index == 0)
      throw std::logic_error{"Array iterator not decrementable"};
#endif // _DEBUG
    --_index;
    return *this;
  }

  auto operator --(int)
  {
    auto temp = *this;

    --* this;
    return temp;
  }

  [[nodiscard]] reference operator *() const
  {
#ifdef _DEBUG
    if (!_array || _index >= _array->size())
      throw std::logic_error{"Array iterator not dereferencable"};
#endif // _DEBUG
    return (*_array)[_index];
  }

  [[nodiscard]] auto operator ->() const
  {
    return &(operator *());
  }

  [[nodiscard]] auto index() const
  {
    return _index;
  }

private:
  const Array* _array{};
  size_t _index{};

}; // ArrayIterator


/////////////////////////////////////////////////////////////////////
//
// ArrayBase: array base class
// =========
template <typename T, typename Allocator>
class ArrayBase
{
public:
  ~ArrayBase()
  {
    release();
  }

  ArrayBase() = default;

  ArrayBase(size_t size):
    _data{Allocator::template allocate<T>(size)},
    _size{size}
  {
    // do nothing
  }

  ArrayBase(const ArrayBase&) = delete;
  ArrayBase& operator =(const ArrayBase&) = delete;

  ArrayBase(ArrayBase&& other) noexcept:
    _data{std::exchange(other._data, nullptr)},
    _size{std::exchange(other._size, 0)}
  {
    // do nothing
  }

  auto& operator =(ArrayBase&& other) noexcept
  {
    if (this != &other)
    {
      release();
      _data = std::exchange(other._data, nullptr);
      _size = std::exchange(other._size, 0);
    }
    return *this;
  }

  [[nodiscard]] auto size() const
  {
    return _size;
  }

  [[nodiscard]] const T* data() const
  {
    return _data;
  }

  [[nodiscard]] T* data()
  {
    return _data;
  }

protected:
  T* _data{};
  size_t _size{};

private:
  void release() noexcept
  {
    Allocator::template free<T>(_data);
  }

}; // ArrayBase


/////////////////////////////////////////////////////////////////////
//
// ArrayAllocator: standard array allocator class
// ==============
class ArrayAllocator
{
public:
  template <typename T>
  [[nodiscard]] static T* allocate(size_t count)
  {
    return new T[count];
  }

  template <typename T>
  static void free(T* ptr)
  {
    delete []ptr;
  }

}; // ArrayAllocator


/////////////////////////////////////////////////////////////////////
//
// Array: array class
// =====
template <typename T, typename Allocator = ArrayAllocator>
class Array: public ArrayBase<T, Allocator>
{
public:
  using value_type = T;
  using array_type = Array<T, Allocator>;

  using ArrayBase<T, Allocator>::ArrayBase;

  auto& copy(const Array& other)
  {
    if (this != &other)
    {
      if (this->_size != other._size)
        throw std::logic_error{"Bad array size"};
      if (this->_size)
        if constexpr (std::is_trivially_copyable_v<T>)
          std::memcpy(this->_data, other._data, this->_size * sizeof(T));
        else
          for (size_t i = 0; i < this->_size; ++i)
            this->_data[i] = other._data[i];
    }
    return *this;
  }

  auto& zero()
  {
    static_assert(std::is_trivially_copyable_v<T>);
    if (this->_size)
      std::memset(this->_data, 0, this->_size * sizeof(T));
    return *this;
  }

  template <IsArrayIndex I>
  [[nodiscard]] const auto& operator [](I index) const
  {
    checkIndex(index);
    return this->_data[index];
  }
 
  template <IsArrayIndex I>
  [[nodiscard]] auto& operator [](I index)
  {
    checkIndex(index);
    return this->_data[index];
  }

  [[nodiscard]] auto begin() const
  {
    return ArrayIterator<Array>{this, 0};
  }

  [[nodiscard]] auto end() const
  {
    return ArrayIterator<Array>{this, this->_size};
  }

private:
  template <IsArrayIndex I>
  void checkIndex(I index) const
  {
#ifdef _DEBUG
    if (static_cast<size_t>(index) < this->_size)
      if constexpr (!std::is_signed_v<I>)
        return;
      else if (index >= 0)
        return;
    throw std::logic_error{"Array index out of bounds"};
#endif // _DEBUG
  }

}; // Array

} // end namespace cg

#endif // __Array_h
