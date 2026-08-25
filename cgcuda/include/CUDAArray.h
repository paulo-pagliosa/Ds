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
// OVERVIEW: CUDAArray.h
// ========
// Classes for host and CUDA arrays.
//
// Author: Paulo Pagliosa
// Last revision: 24/08/2026

#ifndef __CUDAArray_h
#define __CUDAArray_h

#include "core/Array.h"
#include "core/SoA.h"
#ifdef _USE_CUDA
#include "CUDAHelper.h"
#endif // _USE_CUDA

namespace cg
{ // begin namespace cg

#ifdef _USE_CUDA

namespace cuda
{ // begin namespace cuda

template <typename T> class Array;


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
  static void copy(T* dst, const T* src, size_t count, cudaStream_t)
  {
    copyToDevice<T>(dst, src, count);
  }

}; // CopyArrayToDevice

template <typename T>
struct CopyArrayToDevice<T, HostArrayAllocator>
{
  static void copy(T* dst, const T* src, size_t count, cudaStream_t stream)
  {
    copyToDeviceAsync<T>(dst, src, count, stream);
  }

}; // CopyArrayToDevice

template <typename Allocator, typename T>
inline void
copyArrayToDevice(T* dst, const T* src, size_t count, cudaStream_t stream = 0)
{
  CopyArrayToDevice<T, Allocator>::copy(dst, src, count, stream);
}

template <typename Allocator, typename index_t, typename... Args>
struct CopySoAToDevice
{
  using CUDASoA = SoA<ArrayAllocator, index_t, Args...>;
  using HostSoA = SoA<Allocator, index_t, Args...>;

  static void copy(CUDASoA& ds, const HostSoA& hs, cudaStream_t stream)
  {
    ds.reallocate(hs.size());
    CopySoAToDevice::template copyArrays(ds, hs, stream);
  }

  template <size_t I>
    requires (I < sizeof...(Args))
  static void copyArray(CUDASoA& ds, const HostSoA& hs, cudaStream_t stream)
  {
    assert(ds.size() == hs.size());
    CopySoAToDevice::template copyArrays<I, I + 1>(ds, hs, stream);
  }

private:
  template <size_t I = 0, size_t N = sizeof...(Args)>
  static void copyArrays(CUDASoA& ds, const HostSoA& hs, cudaStream_t stream)
  {
    if constexpr (I < N)
    {
      auto dst = ds.template data<I>();

      using D = std::remove_cvref_t<decltype(*dst)>;
      static_assert(std::is_trivially_copyable_v<D>);

      auto src = hs.template data<I>();

      copyArrayToDevice<Allocator>(dst, src, hs.size(), stream);
      CopySoAToDevice::template copyArrays<I + 1, N>(ds, hs, stream);
    }
  }

}; // CopySoAToDevice

template <typename Allocator, typename index_t, typename... Args>
inline void
copySoAToDevice(SoA<ArrayAllocator, index_t, Args...>& ds,
  const SoA<Allocator, index_t, Args...>& hs,
  cudaStream_t stream = 0)
{
  CopySoAToDevice<Allocator, index_t, Args...>::copy(ds, hs, stream);
}

template <typename T, typename Allocator> struct CopyArrayToHost;

template <typename T>
struct CopyArrayToHost<T, cg::ArrayAllocator>
{
  static void copy(T* dst, const T* src, size_t count, cudaStream_t)
  {
    copyToHost<T>(dst, src, count);
  }

}; // CopyArrayToHost

template <typename T>
struct CopyArrayToHost<T, HostArrayAllocator>
{
  static void copy(T* dst, const T* src, size_t count, cudaStream_t stream)
  {
    copyToHostAsync<T>(dst, src, count, stream);
  }

}; // CopyArrayToHost

template <typename Allocator, typename T>
inline void
copyArrayToHost(T* dst, const T* src, size_t count, cudaStream_t stream = 0)
{
  CopyArrayToHost<T, Allocator>::copy(dst, src, count, stream);
}

template <size_t I, typename Allocator, typename index_t, typename... Args>
  requires (I < sizeof...(Args))
inline void
copyArrayToHost(SoA<Allocator, index_t, Args...>& hs,
  const SoA<ArrayAllocator, index_t, Args...>& ds,
  cudaStream_t stream = 0)
{
  assert(hs.size() == ds.size());
  copyArrayToHost<Allocator>(hs.template data<I>(),
    ds.template data<I>(),
    ds.size(),
    stream);
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
  using Base = ArrayBase<T, ArrayAllocator>;

  using Base::ArrayBase;

  template <typename Allocator>
  Array(const cg::Array<T, Allocator>& other, cudaStream_t stream = 0):
    Base{other.size()}
  {
    static_assert(std::is_trivially_copyable_v<T>);
    copyArrayToDevice<Allocator>(this->_data,
      other.data(),
      other.size(),
      stream);
  }

  auto& copy(const Array& other)
  {
    static_assert(std::is_trivially_copyable_v<T>);
    if (this != &other)
    {
      assert(this->_size == other._size);
      deviceCopy<T>(this->_data, other._data, this->_size);
    }
    return *this;
  }

  auto& copy(const Array& other, cudaStream_t stream)
  {
    static_assert(std::is_trivially_copyable_v<T>);
    if (this != &other)
    {
      assert(this->_size == other._size);
      deviceCopyAsync<T>(this->_data, other._data, this->_size, stream);
    }
    return *this;
  }

  auto& zero()
  {
    static_assert(std::is_trivially_copyable_v<T>);
    deviceSet(this->_data, 0, this->_size * sizeof(T));
    return *this;
  }

  auto& zero(cudaStream_t stream)
  {
    static_assert(std::is_trivially_copyable_v<T>);
    deviceSetAsync(this->_data, 0, this->_size * sizeof(T), stream);
    return *this;
  }

}; // Array

using IntArray = Array<int>;

} // end namespace cuda

#endif // _USE_CUDA

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
  using Base = cg::Array<T, Allocator>;

  using Base::Array;

#ifdef _USE_CUDA
  Array(const cuda::Array<T>& other):
    Base{other.size()}
  {
    static_assert(std::is_trivially_copyable_v<T>);
    cuda::copyArrayToHost<Allocator>(this->_data, other.data(), other.size());
  }
#endif // _USE_CUDA

}; // Array

using IntArray = Array<int>;

#ifdef _USE_CUDA

using HostArrayAllocator = cuda::HostArrayAllocator;

template <typename T>
class Array<T, HostArrayAllocator>: public cg::Array<T, HostArrayAllocator>
{
public:
  using value_type = T;
  using Base = cg::Array<T, HostArrayAllocator>;

  using Base::Array;

  Array(const cuda::Array<T>& other, cudaStream_t stream = 0):
    Base{other.size()}
  {
    static_assert(std::is_trivially_copyable_v<T>);
    cuda::copyArrayToHost<HostArrayAllocator>(this->_data,
      other.data(),
      other.size(),
      stream);
  }

}; // Array

#endif // _USE_CUDA

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
  using SoABase = SoABase<index_t, Args...>;

  using Base::SoA;

  template <typename Allocator>
  SoA(const cg::SoA<Allocator, index_t, Args...>& other,
    cudaStream_t stream = 0)
  {
    copySoAToDevice(*this, other, stream);
  }

}; // SoA

} // end namespace cuda

#endif // _USE_CUDA

} // end namespace cg

#endif // __CUDAArray_h
