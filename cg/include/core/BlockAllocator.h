//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2019 Orthrus Group.                         |
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
// OVERVIEW: BlockAllocator.h
// ========
// Class definition for block allocator.
//
// Author: Paulo Pagliosa
// Last revision: 16/02/2019

#ifndef __BlockAllocator_h
#define __BlockAllocator_h

#include <mutex>
#include <string>

namespace cg
{ // begin namespace cg

namespace internal
{ // begin namespace internal

inline size_t
roundupVoidPtr(size_t size)
{
  return (size + sizeof(void*) - 1) & -signed(sizeof(void*));
}

} // end namespace internal


/////////////////////////////////////////////////////////////////////
//
// BlockStorage: block storage class
// ============
class BlockStorage
{
public:
#ifdef _DEBUG
  std::string typeName;
#endif

  /**
   * \brief Constructs a block storage whose blocks can
   * store \p maxSize chunks of size \p requestedSize.
   */
  BlockStorage(size_t requestedSize, unsigned maxSize):
    _chunkSize{internal::roundupVoidPtr(requestedSize)},
    _maxSize{maxSize}
  {
    _nextChunk = _blockSize = _chunkSize * maxSize;
  }

  ~BlockStorage();

  /**
   * \brief Allocates a chunk of memory.
   */
  void* allocate();

  /**
   * \brief Deallocates the chunk of memory pointed by
   * \p ptr.
   */
  void free(void* ptr)
  {
    nextOf(ptr) = _freeList;
    _freeList = ptr;
  }

  int blockCount() const
  {
    return _blockCount;
  }

protected:
  struct Block;

  size_t _chunkSize;
  unsigned _maxSize;
  size_t _blockSize;
  int _blockCount{};
  void* _freeList{};
  Block* _headBlock{};
  Block* _lastBlock{};
  size_t _nextChunk;

  void sortFreeList()
  {
    sort(_freeList);
  }

  static void*& nextOf(void* ptr)
  {
    return *(void**)ptr;
  }

private:
  Block* allocateBlock();

  static void sort(void*&);
  static void* merge(void*, void*);

}; // BlockStorage

struct BlockStorage::Block
{
  void* operator new(size_t size, size_t blockSize)
  {
    return ::operator new(size + blockSize);
  }

  void operator delete(void* ptr)
  {
    ::operator delete(ptr);
  }

  void operator delete(void* ptr, size_t)
  {
    ::operator delete(ptr);
  }

  Block* _next;

  Block(Block* next = nullptr):
    _next{next}
  {
    // do nothing
  }

  static void insert(Block*&, Block*);

}; // BlockStorage::Block


/////////////////////////////////////////////////////////////////////
//
// SingleBlockStorage: single block storage class
// ==================
template <typename T, unsigned size>
class SingletonBlockStorage
{
public:
  static T* allocate()
  {
    storage_type& s = storage();

    s.lock();

    auto ptr = static_cast<T*>(s.allocate());

    s.unlock();
    return ptr;
  }

  static void free(T* ptr)
  {
    storage_type& s = storage();

    s.lock();
    s.free(ptr);
    s.unlock();
  }

  static int blockCount()
  {
    storage_type& s = storage();

    s.lock();

    auto count = s.blockCount();

    s.unlock();
    return count;
  }

private:
  struct storage_type: public std::mutex, BlockStorage
  {
    storage_type():
      BlockStorage{sizeof(T), size}
    {
#ifdef _DEBUG
      typeName = typeid(T).name();
#endif
    }

  }; // storage_type

  static storage_type& storage()
  {
    static storage_type* s;

    if (s == nullptr)
      s = new storage_type();
    // The following line does nothing else than force the
    // instantiation of SingletonBlockStorage<T, size>::creator,
    // whose constructor is called before main() begins.
    creator.execute();
    return *s;
  }

  struct SingletonCreator
  {
    SingletonCreator()
    {
      SingletonBlockStorage<T, size>::storage();
    }

    void execute()
    {
      // do nothing
    }

  }; // SingletonCreator

  static SingletonCreator creator;

}; // SingletonBlockStorage

template <typename T, unsigned size>
typename SingletonBlockStorage<T, size>::SingletonCreator
SingletonBlockStorage<T, size>::creator;


/////////////////////////////////////////////////////////////////////
//
// BlockAllocator: block allocator class
// ==============
template <typename T, unsigned size>
class BlockAllocator
{
public:
  using value_type = T;

  BlockAllocator() = delete;

  /**
   * \brif Allocates memory for an object of type T.
   */
  static value_type* allocate()
  {
    return SingletonBlockStorage<T, size>::allocate();
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
  static value_type* construct(Args&&... args)
  {
    auto ptr = allocate();

    try
    {
      ::new (ptr) T(std::forward<Args>(args)...);
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
  static void free(value_type* ptr)
  {
    SingletonBlockStorage<T, size>::free(ptr);
  }

  /**
   * \brief Destroys the object pointed by \p ptr.
   */
  static void destroy(value_type* ptr)
  {
    ptr->~T();
    free(ptr);
  }

}; // BlockAllocator

// Block allocator for void.
template <unsigned size>
class BlockAllocator<void, size>
{
public:
  using value_type = void;

}; // BlockAllocator

} // end namespace cg

#endif // __BlockAllocator_h
