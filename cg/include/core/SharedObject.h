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
// OVERVIEW: SharedObject.h
// ========
// Class definition for shared object.
//
// Author: Paulo Pagliosa
// Last revision: 14/07/2023

#ifndef __SharedObject_h
#define __SharedObject_h

#include <concepts>

namespace cg
{ // begin namespace cg

//
// Forward definition
//
class SharedObject;

template <typename T>
inline constexpr bool
isSharedObject()
{
  return std::is_assignable_v<T, SharedObject>;
}

template <typename T>
concept SharedObjectType = std::derived_from<T, SharedObject>;

#define ASSERT_SHARED(T, msg) static_assert(SharedObjectType<T>, msg)


/////////////////////////////////////////////////////////////////////
//
// SharedObject: shared object class
// ============
class SharedObject
{
public:
  /// Destructor.
  virtual ~SharedObject() = default;

  /// Returns the number of references of this object.
  auto referenceCount() const
  {
    return _referenceCount;
  }

  template <typename T>
  static auto makeUse(const T* ptr)
  {
    ASSERT_SHARED(T, "Pointer to shared object expected");
    if (ptr != nullptr)
      ++ptr->_referenceCount;
    return (T*)ptr;
  }

  template <typename T>
  static void release(T* ptr)
  {
    ASSERT_SHARED(T, "Pointer to shared object expected");
    if (ptr != nullptr && --ptr->_referenceCount <= 0)
      delete ptr;
  }

protected:
  /// Constructs an unreferenced object.
  SharedObject() = default;

private:
  mutable int _referenceCount{};
  
}; // SharedObject


/////////////////////////////////////////////////////////////////////
//
// Reference: shared object reference class
// =========
template <typename T>
class Reference
{
public:
  using value_type = T;

  Reference():
    _ptr{nullptr}
  {
    // do nothing
  }

  Reference(const Reference& other):
    _ptr{T::makeUse(other._ptr)}
  {
    // do nothing
  }

  Reference(Reference&& other) noexcept:
    _ptr{other._ptr}
  {
    other._ptr = nullptr;
  }

  Reference(const T* ptr):
    _ptr{T::makeUse(ptr)}
  {
    // do nothing
  }

  ~Reference()
  {
    T::release(_ptr);
  }

  auto& operator =(const Reference& other)
  {
    return operator =(other._ptr);
  }

  auto& operator =(Reference&& other) noexcept
  {
    std::swap(_ptr, other._ptr);
    return *this;
  }

  auto& operator =(const T* ptr)
  {
    T::release(_ptr);
    _ptr = T::makeUse(ptr);
    return *this;
  }

  bool operator ==(const Reference& other) const
  {
    return operator ==(other._ptr);
  }

  bool operator ==(const T* ptr) const
  {
    return _ptr == ptr;
  }

  bool operator !=(const Reference& other) const
  {
    return !operator ==(other);
  }

  bool operator !=(const T* ptr) const
  {
    return !operator ==(ptr);
  }

  operator T*() const
  {
    return _ptr;
  }

  auto operator->() const
  {
    return _ptr;
  }

  auto get() const
  {
    return _ptr;
  }

  auto& operator *() const
  {
    return *_ptr;
  }

private:
  T* _ptr;

}; // Reference

} // end namespace cg

#endif // __SharedObject_h
