//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2023 Paulo Pagliosa.                        |
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
// OVERVIEW: AllocableObject.h
// ========
// Class definition for allocable object.
//
// Author: Paulo Pagliosa
// Last revision: 30/07/2023

#ifndef __AllocableObject_h
#define __AllocableObject_h

#include "StandardAllocator.h"
#include <cstddef>
#ifdef _DEBUG
#include <stdexcept>
#include <type_traits>
#endif // _DEBUG

namespace cg
{ // begin namespace cg

#ifdef _DEBUG
template <typename T, typename Allocator>
inline constexpr bool
isStandardAllocator()
{
  return std::is_assignable_v<StandardAllocator<T>, Allocator>;
}
#endif // _DEBUG


/////////////////////////////////////////////////////////////////////
//
// AllocableObject: allocable object class
// ===============
template <typename T, typename Allocator = StandardAllocator<T>>
class AllocableObject
{
public:
  using allocator = Allocator;

  static T* allocate()
  {
    return allocator::allocate();
  }

  static void free(T* ptr)
  {
    allocator::free(ptr);
  }

  void* operator new(size_t size)
  {
#ifdef _DEBUG
    if (size != sizeof(T))
      throw std::logic_error{"Object bad size"};
#endif // _DEBUG
    (void)size;
    return allocator::allocate();
  }

  void operator delete(void* ptr)
  {
    allocator::free((T*)ptr);
  }

}; // AllocableObject

} // end namespace cg

#endif // __AllocableObject_h
