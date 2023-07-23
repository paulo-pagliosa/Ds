//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2021, 2023 Paulo Pagliosa.                        |
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
// Last revision: 19/07/2023

#ifndef __Array_h
#define __Array_h

#include <stdexcept>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// ArrayIterator: host array iterator class
// =============
template <typename Array>
class ArrayIterator
{
public:
  using iterator = ArrayIterator<Array>;
  using value_type = typename Array::value_type;

  ArrayIterator(const Array* array, size_t index):
    _array{array},
    _index{index}
  {
    // do nothing
  }

  bool operator ==(const iterator& other) const
  {
    return _index == other._index && _array == other._array;
  }

  bool operator !=(const iterator& other) const
  {
    return !operator ==(other);
  }

  iterator& operator ++()
  {
#ifdef _DEBUG
    if (_index >= _array->size())
      throw std::logic_error{"Array iterator not incrementable"};
#endif // _DEBUG
    ++_index;
    return *this;
  }

  auto operator ++(int)
  {
    iterator temp(*this);

    ++* this;
    return temp;
  }

  iterator& operator --()
  {
#ifdef _DEBUG
    if (_index == 0)
      throw std::logic_error{"Array iterator not decrementable"};
#endif // _DEBUG
    --_index;
    return *this;
  }

  auto operator --(int)
  {
    iterator temp(*this);

    --* this;
    return temp;
  }

  const value_type& operator *() const
  {
#ifdef _DEBUG
    if (_index >= _array->size())
      throw std::logic_error{"Array iterator not dereferencable"};
#endif // _DEBUG
    return (*_array)[_index];
  }

  auto operator ->() const
  {
    return &(operator *());
  }

  auto index() const
  {
    return _index;
  }

private:
  const Array* _array;
  size_t _index;

}; // ArrayIterator


/////////////////////////////////////////////////////////////////////
//
// ArrayBase: array base class
// =========
template <typename T, typename Allocator>
class ArrayBase
{
public:
  using base_type = ArrayBase<T, Allocator>;

  ~ArrayBase()
  {
    Allocator::template free<T>(_data);
  }

  ArrayBase() = default;

  ArrayBase(size_t size):
    _data{Allocator::template allocate<T>(size)},
    _size{size}
  {
    // do nothing
  }

  ArrayBase(base_type&& other) noexcept:
    _data{other._data},
    _size{other._size}
  {
    other._data = nullptr;
    other._size = 0;
  }

  auto& operator =(base_type&& other) noexcept
  {
    if (this != &other)
    {
      this->~ArrayBase();
      _data = other._data;
      _size = other._size;
      other._data = nullptr;
      other._size = 0;
    }
    return *this;
  }

  auto size() const
  {
    return _size;
  }

  const auto data() const
  {
    return _data;
  }

  auto data()
  {
    return _data;
  }

  operator const T*() const
  {
    return _data;
  }

  operator T*()
  {
    return _data;
  }

protected:
  T* _data{};
  size_t _size{};

  ArrayBase(const base_type&) = delete;
  base_type& operator =(const base_type&) = delete;

}; // ArrayBase


/////////////////////////////////////////////////////////////////////
//
// ArrayAllocator: standard array allocator class
// ==============
class ArrayAllocator
{
public:
  template <typename T>
  static T* allocate(size_t count)
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
// Array: host array class
// =====
template <typename T, typename Allocator = ArrayAllocator>
class Array: public ArrayBase<T, Allocator>
{
public:
  using value_type = T;
  using array_type = Array<T>;
  using ArrayBase<T, Allocator>::ArrayBase;

  auto& copy(const array_type& other)
  {
    if (this != &other)
    {
#ifdef _DEBUG
      if (this->_size != other._size)
        throw std::logic_error{"Bad array size"};
#endif // _DEBUG
      memcpy(this->_data, other._data, this->_size * sizeof(T));
    }
    return *this;
  }

  auto& zero()
  {
    memset(this->_data, 0, this->_size * sizeof(T));
    return *this;
  }

  const auto& operator [](size_t index) const
  {
#ifdef _DEBUG
    if (index >= this->_size)
      throw std::logic_error{"Array index out of bounds"};
#endif // _DEBUG
    return this->_data[index];
  }

  auto& operator [](size_t index)
  {
#ifdef _DEBUG
    if (index >= this->_size)
      throw std::logic_error{"Array index out of bounds"};
#endif // _DEBUG
    return this->_data[index];
  }

  auto begin() const
  {
    return ArrayIterator<array_type>{this, 0};
  }

  auto end() const
  {
    return ArrayIterator<array_type>{this, this->_size};
  }

}; // Array

} // end namespace cg

#endif // __Array_h
