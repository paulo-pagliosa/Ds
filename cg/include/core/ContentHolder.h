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
// OVERVIEW: ContentHolder.h
// ========
// Class definition for content holder.
//
// Author: Paulo Pagliosa
// Last revision: 08/08/2023

#ifndef __ContentHolder_h
#define __ContentHolder_h

#include <type_traits>

namespace cg
{ // begin namespace cg

template <typename T>
inline constexpr bool
isNotVoid()
{
  return !std::is_void_v<T>;
}

#define ASSERT_NOT_VOID(T, msg) static_assert(isNotVoid<T>(), msg)


/////////////////////////////////////////////////////////////////////
//
// ContentHolder: content holder class
// =============
template <typename T>
class ContentHolder
{
public:
  using data_type = T;

  ContentHolder() = default;

  ContentHolder(const T& data):
    _data{data}
  {
    // do nothing
  }

  ContentHolder(T&& data):
    _data{std::move(data)}
  {
    // do nothing
  }

  const auto& data() const
  {
    return _data;
  }

  auto& data()
  {
    return _data;
  }

  void setData(const data_type& data)
  {
    _data = data;
  }

  void setData(data_type&& data)
  {
    _data = std::move(data);
  }

private:
  data_type _data;

}; // ContentHolder

// Content holder of void.
template <>
class ContentHolder<void>
{
public:
  using data_type = void;

}; // ContentHolder

} // end namespace cg

#endif // __ContentHolder_h
