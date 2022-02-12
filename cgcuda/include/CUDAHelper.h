//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2014, 2021 Paulo Pagliosa.                        |
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
// OVERVIEW: CUDAHelper.h
// ========
// Classes and functions for CUDA utilities.
//
// Author: Paulo Pagliosa
// Last revision: 26/04/2021

#ifndef __CUDAHelper_h
#define __CUDAHelper_h

#include "core/Globals.h"
#include <iostream>

namespace cg
{ // begin namespace cg

namespace cuda
{ // begin namespace cuda

void error(const char*, ...);
void checkError(cudaError_t, const char*, int);
void checkLastError(const char*, const char*, int);

#define checkCudaError(err) cuda::checkError(err, __FILE__, __LINE__)
#define checkLastCudaError(msg) cuda::checkLastError(msg, __FILE__, __LINE__)

inline void
reset()
{
  cudaDeviceReset();
}

inline void
initialize(int device = -1)
{
  int count;

  checkCudaError(cudaGetDeviceCount(&count));
  if (count == 0)
    error("No devices supporting CUDA");
  if (device < 0)
    device = 0;
  else if (device > count - 1)
    error("Device %d is not a valid GPU.\n", device);

  cudaDeviceProp deviceProp;

  checkCudaError(cudaGetDeviceProperties(&deviceProp, device));
  if (deviceProp.computeMode == cudaComputeModeProhibited)
    error("Device %d is running in compute mode prohibited", device);
  if (deviceProp.major < 1)
    error("Device %d does not support CUDA", device);
  checkCudaError(cudaSetDevice(device));
  printf("Using CUDA device %d: %s\n", device, deviceProp.name);
}

inline int
currentDevice()
{
  int device;

  checkCudaError(cudaGetDevice(&device));
  return device;
}

inline void
synchronize()
{
  checkCudaError(cudaDeviceSynchronize());
}

inline void
allocate_v(void*& ptr, size_t size)
{
  checkCudaError(cudaMalloc((void**)&ptr, size));
}

inline void
free_v(void* ptr)
{
  checkCudaError(cudaFree(ptr));
}

template <typename T>
inline void
allocate(T*& ptr, size_t count)
{
  allocate_v((void*&)ptr, count * sizeof(T));
}

template <typename T>
inline void
free(T*& ptr)
{
  free_v((void*&)ptr);
}

inline void
copyToHost_v(void* dst, const void* src, size_t size)
{
  checkCudaError(cudaMemcpy(dst, src, size, cudaMemcpyDeviceToHost));
}

template <typename T>
inline void
copyToHost(T* dst, const T* src, size_t count)
{
  copyToHost_v((void*)dst, (const void*)src, count * sizeof(T));
}

inline void
copyToDevice_v(void* dst, const void* src, size_t size)
{
  checkCudaError(cudaMemcpy(dst, src, size, cudaMemcpyHostToDevice));
}

template <typename T>
inline void
copyToDevice(T* dst, const T* src, size_t count)
{
  copyToDevice_v((void*)dst, (const void*)src, count * sizeof(T));
}

template <typename T>
inline void
newCopyToDevice(T*& dst, const T* src, size_t n)
{
  allocate<T>(dst, n);
  copyToDevice<T>(dst, src, n);
}

inline void
deviceCopy_v(void* dst, const void* src, size_t size)
{
  checkCudaError(cudaMemcpy(dst, src, size, cudaMemcpyDeviceToDevice));
}

template<typename T>
inline void
deviceCopy(T* dst, const T* src, size_t count)
{
  deviceCopy_v((void*)dst, (const void*)src, count * sizeof(T));
}

inline void
deviceSet(void* ptr, int value, size_t size)
{
  checkCudaError(cudaMemset(ptr, value, size));
}

inline void
copyToSymbol_v(const void* dst, const void* src, size_t size)
{
  checkCudaError(cudaMemcpyToSymbol(dst, src, size));
}

template <typename T>
inline void
copyToSymbol(const T* dst, const T* src, size_t count)
{
  copyToSymbol_v((const void*)dst, (const void*)src, sizeof(T) * count);
}

template <typename T>
inline void
copyToSymbol(const T& dst, const T& src)
{
  copyToSymbol<T>(&dst, &src, 1);
}

inline void
copyFromSymbol_v(void* dst, const void* src, size_t size)
{
  checkCudaError(cudaMemcpyFromSymbol(dst, src, size));
}

template <typename T>
inline void
copyFromSymbol(T* dst, const T* src, size_t count)
{
  copyFromSymbol_v((void*)dst, (const void*)src, sizeof(T) * count);
}

template <typename T>
inline void
copyFromSymbol(T& dst, const T& src)
{
  copyFromSymbol<T>(&dst, &src, 1);
}

inline void*
address_v(const void* symbol)
{
  void* ptr;

  checkCudaError(cudaGetSymbolAddress(&ptr, symbol));
  return ptr;
}

template <typename T>
inline T*
address(const T* symbol)
{
  return (T*)address_v(symbol);
}

template <typename T>
void
dump(const char* s, const T* data, size_t count, std::ostream& os = std::cout)
{
  T* hData = new T[count];

  copyToHost<T>(hData, data, count);
  os << s << ": ";
  for (size_t i = 0; i < count; ++i)
    os << hData[i] << ' ';
  os << '\n';
  delete []hData;
}


/////////////////////////////////////////////////////////////////////
//
// Buffer: CUDA buffer class
// ======
template <typename T>
class Buffer
{
public:
  using value_type = T;

  Buffer() = default;

  Buffer(size_t size, const T* hData = nullptr)
  {
    if (size == 0)
      throw std::logic_error("cuda::Buffer ctor: bad size");
    allocate<T>(_data, _size = size);
    if (hData != nullptr)
      copyToDevice<T>(_data, hData, size);
  }

  Buffer(const Buffer<T>&) = delete;
  Buffer<T>& operator =(const Buffer<T>&) = delete;

  Buffer(Buffer<T>&& other):
    _data{other._data},
    _size{other._size}
  {
    other._data = nullptr;
    other._size = 0;
  }

  ~Buffer()
  {
    free<T>(_data);
    _size = 0;
  }

  void copy(const T*, const T*, size_t);

  void fill(const T&, size_t, size_t);

  void zero()
  {
    deviceSet(_data, 0, _size * sizeof(T));
  }

  Buffer<T>& operator =(Buffer<T>&& other) noexcept
  {
    free<T>(_data);
    _data = other._data;
    _size = other._size;
    other._data = nullptr;
    other._size = 0;
    return *this;
  }

  auto size() const
  {
    return _size;
  }

  operator const value_type*() const
  {
    return _data;
  }

  operator value_type*()
  {
    return _data;
  }

  void dump(const char* s, std::ostream& os = std::cout) const
  {
    cuda::dump<T>(s, _data, _size, os);
  }

private:
  value_type* _data{};
  size_t _size{};

}; // Buffer

template <typename T>
void
Buffer<T>::copy(const T* hBegin, const T* hEnd, size_t offset)
{
  uint32_t length = static_cast<uint32_t>(hEnd - hBegin);

  if (offset + length > _size)
    throw std::logic_error("cuda::Buffer copy: bad range");
  copyToDevice<T>(_data + offset, hBegin, length);
}

template <typename T>
void
Buffer<T>::fill(const T& value, size_t offset, size_t length)
{
  if (offset + length > _size)
    throw std::logic_error("cuda::Buffer fill: bad range");

  T* hData = new T[length];

  std::fill(hData, hData + length, value);
  copyToDevice<T>(_data + offset, hData, length);
  delete []hData;
}

} // end namespace cuda

} // end namespace cg

#endif // __CUDAHelper_h
