//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: Object.h
// ========
// Class definition for generic vis object.
//
// Author: Paulo Pagliosa
// Last revision: 15/03/2022

#ifndef __VisObject_h
#define __VisObject_h

#include "core/SharedObject.h"
#include <cinttypes>

namespace cg::vis
{ // begin namespace cg::vis

//
// Forward definition
//
class Object;

template <typename T>
inline constexpr bool isObject()
{
  return std::is_assignable<Object, T>::value;
}

#define ASSERT_OBJECT(T, msg) static_assert(isObject<T>(), msg)


/////////////////////////////////////////////////////////////////////
//
// Timestamp: timestamp class
// =========
class Timestamp
{
public:
  using value_type = uint64_t;

  auto modifiedTime() const
  {
    return _modifiedTime;
  }

  void modified()
  {
    static value_type time;
    _modifiedTime = ++time;
  }

  void reset()
  {
    _modifiedTime = value_type{};
  }

  bool operator <(const Timestamp& other) const
  {
    return _modifiedTime < other._modifiedTime;
  }

  bool operator >(const Timestamp& other) const
  {
    return _modifiedTime > other._modifiedTime;
  }

private:
  value_type _modifiedTime{};

}; // Timestamp


/////////////////////////////////////////////////////////////////////
//
// Object: generic vis object class
// ======
class Object: public virtual SharedObject
{
public:
  Object()
  {
    _modifiedTime.modified();
  }

  template <typename T>
  static T* makeCopy(T* ptr)
  {
    ASSERT_OBJECT(T, "Pointer to object expected");
    return ptr == nullptr ? nullptr : ptr->clone();
  }

  auto modifiedTime() const
  {
    return _modifiedTime;
  }

  virtual void modified()
  {
    _modifiedTime.modified();
  }

private:
  Timestamp _modifiedTime;

}; // Object

} // end namespace cg::vis

#endif // __VisObject_h
