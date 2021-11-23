//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018 Orthrus Group.                               |
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
// OVERVIEW: BlockAllocator.cpp
// ========
// Source file for block allocator.
//
// Author: Paulo Pagliosa
// Last revision: 01/98/2019

#include "core/BlockAllocator.h"
#include <iostream>

namespace cg
{ // begin namespace cg

#define _DEBUG_BLOCKS 0


/////////////////////////////////////////////////////////////////////
//
// BlockStorage implementation
// ============
inline void
BlockStorage::Block::insert(Block*& front, Block* block)
{
  if (front == nullptr || front > block)
  {
    block->_next = front;
    front = block;
    return;
  }

  Block* b = front;

  while (b->_next != nullptr && b->_next < block)
    b = b->_next;
  block->_next = b->_next;
  b->_next = block;
}

BlockStorage::Block*
BlockStorage::allocateBlock()
{
  auto block = new(_blockSize) Block;

  if (block == nullptr)
    return nullptr;
  Block::insert(_headBlock, block);
#if _DEBUG && _DEBUG_BLOCKS > 0
    std::cout << typeName << " BLOCK " << _blockCount << " ALLOCATED\n";
#endif
    ++_blockCount;
  _nextChunk = 0;
  return _lastBlock = block;
}

BlockStorage::~BlockStorage()
{
#if _DEBUG && _DEBUG_BLOCKS > 0
  size_t i = 0;
#endif

  while (auto temp = _headBlock)
  {
    _headBlock = temp->_next;
    delete temp;
#if _DEBUG && _DEBUG_BLOCKS > 0
    std::cout << typeName << " BLOCK " << i++ << " FREED\n";
#endif
  }
}

void*
BlockStorage::allocate()
{
  if (_freeList != nullptr)
  {
    void* ptr = _freeList;

    _freeList = nextOf(ptr);
    return ptr;
  }
  if (_chunkSize > _blockSize - _nextChunk)
    if (allocateBlock() == nullptr)
      return nullptr;

  void* ptr = ((char*)(_lastBlock + 1)) + _nextChunk;

  _nextChunk += _chunkSize;
  return ptr;
}

void
BlockStorage::sort(void*& head)
{
  void* a{head};
  void* b;

  if (a == nullptr || (b = nextOf(a)) == nullptr)
    return;
  {
    void* s{a};
    void* f{b};

    while (f != nullptr)
      if ((f = nextOf(f)) != nullptr)
      {
        s = nextOf(s);
        f = nextOf(f);
      }
    b = nextOf(s);
    nextOf(s) = nullptr;
  }
  sort(a);
  sort(b);
  head = merge(a, b);
}

void*
BlockStorage::merge(void* a, void* b)
{
  if (a == nullptr)
    return b;
  if (b == nullptr)
    return a;

  void* r;

  if (a <= b)
  {
    r = a;
    nextOf(r) = merge(nextOf(a), b);
  }
  else
  {
    r = b;
    nextOf(r) = merge(a, nextOf(b));
  }
  return r;
}

} // end namespace cg
