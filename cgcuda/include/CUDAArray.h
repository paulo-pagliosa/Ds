//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2021, 2025 Paulo Pagliosa.                        |
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
// Last revision: 16/12/2025

#ifndef __CUDAArray_h
#define __CUDAArray_h

#include "core/Array.h"
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
template <typename T, typename Allocator = ArrayAllocator>
class Array: public cg::Array<T, Allocator>
{
public:
  using value_type = T;
  using cg::Array<T>::Array;

  Array(const cuda::Array<T>&);

}; // Array

using IntArray = Array<int>;

} // end namespace host

#ifdef _USE_CUDA

namespace cuda
{ // begin namespace cuda


/////////////////////////////////////////////////////////////////////
//
// HostArrayAllocator: host array allocator class
// ==================
class HostArrayAllocator
{
public:
  template <typename T>
  static T* allocate(size_t count)
  {
    T* ptr;

    cuda::hostAllocate<T>(ptr, count);
    return ptr;
  }

  template <typename T>
  static void free(T* ptr)
  {
    cuda::freeHost<T>(ptr);
  }

}; // HostArrayAllocator


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

template <typename T, typename Allocator> struct CopyArrayToDevice;

template <typename T>
struct CopyArrayToDevice<T, cg::ArrayAllocator>
{
  static void copy(T* dst, const T* src, size_t count)
  {
    copyToDevice<T>(dst, src, count);
  }

}; // CopyArrayToDevice

template <typename T>
struct CopyArrayToDevice<T, HostArrayAllocator>
{
  static void copy(T* dst, const T* src, size_t count)
  {
    copyToDeviceAsync<T>(dst, src, count);
  }

}; // CopyArrayToDevice

template <typename Allocator, typename T>
inline void
copyArrayToDevice(T* dst, const T* src, size_t count)
{
  CopyArrayToDevice<T, Allocator>::copy(dst, src, count);
}

template <typename Allocator, typename index_t, typename... Args>
struct CopySoAToDevice
{
  using CUDASoA = SoA<ArrayAllocator, index_t, Args...>;
  using HostSoA = SoA<Allocator, index_t, Args...>;

  static void copy(CUDASoA& ds, const HostSoA& hs)
  {
    ds.reallocate(hs.size());
    CopySoAToDevice::template copyArrays(ds, hs);
  }

  template <size_t I>
    requires (I < sizeof...(Args))
  static void copyArray(CUDASoA& ds, const HostSoA& hs)
  {
    assert(ds.size() == hs.size());
    CopySoAToDevice::template copyArrays<I, I + 1>(ds, hs);
  }

private:
  template <size_t I = 0, size_t N = sizeof...(Args)>
  static void copyArrays(CUDASoA& ds, const HostSoA& hs)
  {
    if constexpr (I < N)
    {
      auto dst = ds.template data<I>();

      using D = std::remove_cvref_t<decltype(*dst)>;
      static_assert(std::is_trivially_copyable_v<D>);

      auto src = hs.template data<I>();

      cuda::copyArrayToDevice<Allocator>(dst, src, hs.size());
      CopySoAToDevice::template copyArrays<I + 1, N>(ds, hs);
    }
  }

}; // CopySoAToDevice

template <typename Allocator, typename index_t, typename... Args>
inline void
copySoAToDevice(SoA<ArrayAllocator, index_t, Args...>& ds,
  const SoA<Allocator, index_t, Args...>& hs)
{
  CopySoAToDevice<Allocator, index_t, Args...>::copy(ds, hs);
}

template <typename T, typename Allocator> struct CopyArrayToHost;

template <typename T>
struct CopyArrayToHost<T, cg::ArrayAllocator>
{
  static void copy(T* dst, const T* src, size_t count)
  {
    copyToHost<T>(dst, src, count);
  }

}; // CopyArrayToHost

template <typename T>
struct CopyArrayToHost<T, HostArrayAllocator>
{
  static void copy(T* dst, const T* src, size_t count)
  {
    copyToHostAsync<T>(dst, src, count);
  }

}; // CopyArrayToHost

template <typename Allocator, typename T>
inline void
copyArrayToHost(T* dst, const T* src, size_t count)
{
  CopyArrayToHost<T, Allocator>::copy(dst, src, count);
}

template <size_t I, typename Allocator, typename index_t, typename... Args>
  requires (I < sizeof...(Args))
inline void
copyArrayToHost(SoA<Allocator, index_t, Args...>& hs,
  const SoA<ArrayAllocator, index_t, Args...>& ds)
{
  assert(hs.size() == ds.size());
  cuda::copyArrayToHost<Allocator>(hs.template data<I>(),
    ds.template data<I>(),
    ds.size());
}


/////////////////////////////////////////////////////////////////////
//
// Array: CUDA array class
// =====
template <typename T>
class Array: public ArrayBase<T, ArrayAllocator>
{
public:
  using value_type = T;
  using ArrayBase<T, ArrayAllocator>::ArrayBase;

  template <typename Allocator>
  Array(const cg::Array<T, Allocator>& other):
    ArrayBase<T, ArrayAllocator>{other.size()}
  {
    static_assert(std::is_trivially_copyable_v<T>);
    copyArrayToDevice<Allocator>(this->_data, other.data(), other.size());
  }

  auto& copy(const Array& other)
  {
    static_assert(std::is_trivially_copyable_v<T>);
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
    static_assert(std::is_trivially_copyable_v<T>);
    deviceSet(this->_data, 0, this->_size * sizeof(T));
    return *this;
  }

}; // Array

using IntArray = Array<int>;

} // end namespace cuda

namespace host
{ // begin namespace host

template <typename T, typename Allocator>
inline
Array<T, Allocator>::Array(const cuda::Array<T>& other):
  cg::Array<T, Allocator>{other.size()}
{
  static_assert(std::is_trivially_copyable_v<T>);
  cuda::copyArrayToHost<Allocator>(this->_data, other.data(), other.size());
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
  using Base = cg::SoA<ArrayAllocator, index_t, Args...>;

  using Base::SoA;

  template <typename Allocator>
  SoA(const cg::SoA<Allocator, index_t, Args...>& other)
  {
    copySoAToDevice(*this, other);
  }

}; // SoA

} // end namespace cuda

#endif // _USE_CUDA

} // end namespace cg

#endif // __CUDAArray_h
