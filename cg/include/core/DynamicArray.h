//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2026 Paulo Pagliosa.                              |
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
// OVERVIEW: DynamicArray.h
// ========
// Class for generic dynamic array.
//
// Author: Paulo Pagliosa
// Last revision: 23/10/2026

#ifndef __DynamicArray_h
#define __DynamicArray_h

#include "core/Array.h"
#include <cassert>
#include <type_traits>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// DynamicArray: dynamic array class
// ============
template <typename T, typename Allocator = ArrayAllocator>
class DynamicArray: public Array<T, Allocator>
{
public:
  using Base = Array<T, Allocator>;
  using value_type = T;
  using array_type = DynamicArray<T, Allocator>;

  DynamicArray() = default;

  /// Constructs an array with size (and capacity) elements.
  explicit DynamicArray(size_t size):
    Base{size},
    _capacity{size}
  {
    // do nothing
  }

  DynamicArray(DynamicArray&& other) noexcept:
    Base{std::move(other)},
    _capacity{std::exchange(other._capacity, 0)}
  {
    // do nothing
  }

  DynamicArray& operator =(DynamicArray&& other) noexcept
  {
    if (this != &other)
    {
      Base::operator =(std::move(other));
      _capacity = std::exchange(other._capacity, 0);
    }
    return *this;
  }

  [[nodiscard]] auto capacity() const
  {
    return _capacity;
  }

  [[nodiscard]] bool empty() const
  {
    return this->_size == 0;
  }

  void reserve(size_t capacity)
  {
    if (capacity > _capacity)
      reallocate(capacity);
  }

  /// Sets the size to count. Elements added are value-initialized;
  /// the capacity grows geometrically if needed and never shrinks.
  void resize(size_t count)
  {
    if (count > _capacity)
      grow(count);
    setSize(count);
  }

  /// Adds a copy of value; returns its index.
  size_t add(const T& value)
  {
    T temp{value};
    return add(std::move(temp));
  }

  /// Adds value; returns its index.
  size_t add(T&& value)
  {
    const auto i = this->_size;

    if (i == _capacity)
      grow(checkedSum(i, 1));
    this->_data[i] = std::move(value);
    this->_size++;
    return i;
  }

  /// Sets the size to 0 (keeps the capacity).
  void clear()
  {
    setSize(0);
  }

  /// Reduces the capacity to the size.
  void shrinkToFit()
  {
    if (_capacity > this->_size)
      reallocate(this->_size);
  }

private:
  size_t _capacity{};

  static size_t checkedSum(size_t a, size_t b)
  {
    if (b > Base::maxSize)() - a)
      throw std::length_error{"Array: too many elements"};
    return a + b;
  }

  void setSize(size_t n)
  {
    const auto size = this->_size;

    if (n > size)
      std::fill(this->_data + size, this->_data + n, T{});
    else if constexpr (!std::is_trivially_destructible_v<T>)
      std::fill(this->_data + n, this->_data + size, T{});
    this->_size = n;
  }

  void grow(size_t n)
  {
    const auto c = _capacity;
    constexpr auto maxSize = Base::maxSize();
    const auto g = c <= maxSize() / 2 ? c * 2 : maxSize();

    reallocate(std::max({n, g, size_t(8)}));
  }

  // Moves the elements to a new block of capacity >= size()
  void reallocate(size_t);

}; // DynamicArray

template <typename T, typename Allocator>
void
DynamicArray<T, Allocator>::reallocate(size_t n)
{
  assert(n >= this->_size);

  T* data{};

  if (n > 0)
  {
    data = Base::allocate(n);
    if constexpr (std::is_trivially_copyable_v<T>)
    {
      if (this->_size)
        std::memcpy(data, this->_data, this->_size * sizeof(T));
    }
    else
    {
      try
      {
        for (size_t i = 0; i < this->_size; ++i)
          data[i] = std::move_if_noexcept(this->_data[i]);
      }
      catch (...)
      {
        Allocator::free(data);
        throw;
      }
    }
  }
  Allocator::free(this->_data);
  this->_data = data;
  _capacity = n;
}

} // end namespace cg

#endif // __DynamicArray_h
