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
// OVERVIEW: CUDAArray.h
// ========
// Classes for host and CUDA arrays.
//
// Author: Paulo Pagliosa
// Last revision: 08/12/2023

#ifndef __CUDAArray_h
#define __CUDAArray_h

#include "core/SoA.h"
#ifdef _USE_CUDA
#include "CUDAHelper.h"
#endif // _USE_CUDA

namespace cg
{ // begin namespace cg

namespace cuda { template <typename T> class Array; }

namespace host
{ // begin namespace host


/////////////////////////////////////////////////////////////////////
//
// Array: host array class
// =====
template <typename T>
class Array: public cg::Array<T>
{
public:
  using value_type = T;
  using array_type = Array<T>;
  using array_base = cg::Array<T>;
  using array_base::Array;

  Array(const cuda::Array<T>&);

}; // Array

using IntArray = Array<int>;

} // end namespace host

#ifdef _USE_CUDA

namespace cuda
{ // begin namespace cuda


/////////////////////////////////////////////////////////////////////
//
// ArrayAllocator: CUDA array allocator class
// ==============
class ArrayAllocator
{
public:
  template <typename T>
  static T* allocate(size_t count)
  {
    T* ptr;

    cuda::allocate<T>(ptr, count);
    return ptr;
  }

  template <typename T>
  static void free(T* ptr)
  {
    cuda::free<T>(ptr);
  }

}; // ArrayAllocator


/////////////////////////////////////////////////////////////////////
//
// Array: CUDA array class
// =====
template <typename T>
class Array: public ArrayBase<T, ArrayAllocator>
{
public:
  using value_type = T;
  using array_type = Array<T>;
  using ArrayBase<T, ArrayAllocator>::ArrayBase;

  Array(const host::Array<T>& other):
    ArrayBase<T, ArrayAllocator>{other.size()}
  {
    copyToDevice<T>(this->_data, (const T*)other, other.size());
  }

  auto& copy(const array_type& other)
  {
    if (this != &other)
    {
#ifdef _DEBUG
      if (this->_size != other._size)
        throw std::logic_error("Bad array size");
#endif // _DEBUG
      deviceCopy<T>(this->_data, other._data, this->_size);
    }
    return *this;
  }

  auto& zero()
  {
    deviceSet(this->_data, 0, this->_size * sizeof(T));
    return *this;
  }

}; // Array

using IntArray = Array<int>;

} // end namespace cuda

namespace host
{ // begin namespace host

template <typename T>
inline
Array<T>::Array(const cuda::Array<T>& other):
  cg::Array<T>{other.size()}
{
  cuda::copyToHost<T>(this->_data, (const T*)other, other.size());
}

} // end namespace host

#endif // _USE_CUDA

namespace host
{ // begin namespace host

template <typename index_t, typename... Args>
using SoA = cg::SoA<ArrayAllocator, index_t, Args...>;

} // end namespace host

#ifdef _USE_CUDA

namespace cuda
{ // begin namespace cuda


/////////////////////////////////////////////////////////////////////
//
// SoA: CUDA SoA class
// ===
template <typename index_t, typename... Args>
class SoA: public cg::SoA<ArrayAllocator, index_t, Args...>
{
public:
  using type = SoA<index_t, Args...>;
  using cg::SoA<ArrayAllocator, index_t, Args...>::SoA;

  SoA(const host::SoA<index_t, Args...>& other)
  {
    this->reallocate(other.size());
    this->template copyArrayToDevice<0>(other);
  }

private:
  template <size_t I>
  void copyArrayToDevice(const host::SoA<index_t, Args...>& other)
  {
    if constexpr (I < sizeof...(Args))
    {
      auto src = other.template data<I>();
      auto dst = this->template data<I>();

      copyToDevice(dst, src, this->size());
      this->template copyArrayToDevice<I + 1>(other);
    }
  }

}; // SoA

} // end namespace cuda

#endif // _USE_CUDA

} // end namespace cg

#endif // __CUDAArray_h
