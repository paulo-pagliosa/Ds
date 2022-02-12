//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2020 Paulo Pagliosa.                        |
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
// OVERVIEW: StandardAllocator.h
// ========
// Class definition for standard allocator.
//
// Author: Paulo Pagliosa
// Last revision: 28/01/2020

#ifndef __StandardAllocator_h
#define __StandardAllocator_h

#include <utility>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// StandardAllocator: standard allocator class
// =================
template <typename T>
class StandardAllocator
{
public:
  using value_type = T;

  StandardAllocator() = delete;

  /**
   * \brif Allocates memory for an object of type T.
   */
  static value_type* allocate()
  {
    return static_cast<T*>(::operator new(sizeof(T)));
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
  static void free(value_type* ptr)
  {
    ::operator delete(ptr);
  }

  /**
   * \brief Destroys the object pointed by \p ptr.
   */
  static void destroy(value_type* ptr)
  {
    ptr->~T();
    free(ptr);
  }

}; // StandardAllocator

// Standard allocator for void.
template <>
class StandardAllocator<void>
{
public:
  using value_type = void;

}; // StandardAllocator

} // end namespace cg

#endif // __StandardAllocator_h
