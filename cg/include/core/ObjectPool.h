//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018 Paulo Pagliosa.                              |
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
// OVERVIEW: ObjectPool.h
// ========
// Class definition for object pool.
//
// Author: Paulo Pagliosa
// Last revision: 01/09/2018

#ifndef __ObjectPool_h
#define __ObjectPool_h

#include "core/BlockAllocable.h"
#include <iostream>
#include <mutex>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// ObjectPool: object pool class
// ==========
template <typename T>
class ObjectPool: public std::mutex, BlockStorage
{
public:
  using value_type = T;

  struct Stats
  {
    size_t objectCount{};
    size_t totalMemory{};

    friend std::ostream& operator <<(std::ostream& os, const Stats& s)
    {
      os << "Object count: " << s.objectCount << '\n';
      os << "Total memory: " << s.totalMemory << '\n';
      return os;
    }

  }; // Stats

  static const unsigned defaultSize = 64;

  /**
   * \brief Constructs an object pool for objects of
   * type T.
   */
  ObjectPool(unsigned size = defaultSize):
    BlockStorage{sizeof(T), size}
  {
    // do nothing
  }

  ~ObjectPool();

  /**
   * \brief Allocates memory for an object of type T.
   */
  value_type* allocate()
  {
    std::lock_guard<std::mutex> lock{*this};

    if (auto o = static_cast<T*>(BlockStorage::allocate()))
    {
      ++_stats.objectCount;
      _stats.totalMemory += _chunkSize;
      return o;
    }
    return nullptr;
  }

  /**
  * \brief Constructs an object of type T.
  *
  * \returns A pointer to an object of type T, allocated by a
  * call to \ref allocate() and constructed from arguments \p
  * args. The returned objected can be freed and destroyed by
  * a call to \ref destroy().
  */
  template <typename... Args>
  value_type* construct(Args&&... args)
  {
    auto ptr = allocate();

    try
    {
      new (ptr) T(std::forward<Args>(args)...);
    }
    catch (...)
    {
      free(ptr);
      throw;
    }
    return ptr;
  }

  /**
  * \brif Deallocates the memory previously allocated for the
  * object pointed by \p ptr.
  */
  void free(value_type* ptr)
  {
    std::lock_guard<std::mutex> lock{*this};

    BlockStorage::free(ptr);
    --_stats.objectCount;
    _stats.totalMemory -= _chunkSize;
  }

  /**
  * \brief Destroys the object pointed by \p ptr.
  */
  void destroy(value_type* ptr)
  {
    ptr->~T();
    free(ptr);
  }

  Stats stats() const
  {
    return _stats;
  }

private:
  Stats _stats;

}; // ObjectPool

template <typename T>
ObjectPool<T>::~ObjectPool()
{
  if (_freeList == nullptr)
    return;
  sortFreeList();
  for (auto b = _headBlock; b != nullptr; b = b->_next)
  {
    auto s = b == _lastBlock ? _nextChunk : _blockSize;
    auto i = (char*)(b + 1);
    auto e = (char*)(b + 1) + s;

    for (; i != e; i += _chunkSize)
    {
      if ((void*)i == _freeList)
      {
        _freeList = nextOf(_freeList);
        continue;
      }
      // Chunk pointed by i is allocated.
      // Destroy the object stored in the chunk.
      static_cast<T*>((void*)i)->~T();
    }
  }
}

} // end namespace cg

#endif // __ ObjectPool_h
