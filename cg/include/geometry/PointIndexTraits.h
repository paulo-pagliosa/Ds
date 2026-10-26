//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2026 Paulo Pagliosa.                              |
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
// OVERVIEW: PointIndexTraits.h
// ========
// Class definition for traits of the point indices.
//
// Author: Paulo Pagliosa
// Last revision: 26/10/2026

#ifndef __PointIndexTraits_h
#define __PointIndexTraits_h

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

namespace cg
{ // begin namespace cg

template <typename T, typename... Ts>
constexpr bool has = (std::is_same_v<T, Ts> || ...);

template <typename T, typename... Ts>
   requires has<T, Ts...>
constexpr size_t indexOf()
{
  size_t i = 0;

  ((std::is_same_v<T, Ts> ? false : (++i, true)) && ...);
  return i;
}

template <typename... Ts>
constexpr bool distinct()
{
  if constexpr (sizeof...(Ts) == 0)
    return true;
  else
    return[]<typename T, typename... Us>(std::type_identity<T>,
      std::type_identity<Us>...)
  {
    return (!std::is_same_v<T, Us> && ...) && distinct<Us...>();
  }(std::type_identity<Ts>{}...);
}

namespace pa
{ // begin namespace pa

//
// Point attribute accessors
//
// Point position as points[i] (e.g., in a container of positions
// or a PointArray)
struct DefaultPosition
{
  template <typename PA, typename I>
  [[nodiscard]] decltype(auto) operator ()(const PA& points, I i) const
  {
    return points[i];
  }

}; // DefaultPosition

// Field J of a point set with get<J>(i) (e.g., PointArray, whose
// field 0 is the position)
template <size_t J>
struct Field
{
  template <typename PA, typename I>
  [[nodiscard]] decltype(auto) operator ()(const PA& points, I i) const
  {
    return points.template get<J>(i);
  }

}; // Field

namespace internal
{ // begin namespace internal

template <typename F, typename... Args>
using result_of = std::invoke_result_t<F&, Args...>;

// Type of the attribute read by an accessor
template <typename Accessor, typename PA, typename I>
using attribute_type = std::remove_cvref_t<result_of<const Accessor&,
  const PA&,
  I>>;

} // end namespace internal


/////////////////////////////////////////////////////////////////////
//
// SlotCache: slot cache class
// =========
template <typename... Accessors>
struct SlotCache
{
  static_assert(distinct<Accessors...>(),
    "SlotCache: accessors must be distinct types");

  /// Number of cached attributes.
  static constexpr auto size = sizeof...(Accessors);

  /// True if the attribute read by Accessor is cached.
  template <typename Accessor>
  static constexpr bool has = cg::has<Accessor, Accessors...>;

  /// Index of the attribute read by Accessor in the cache.
  template <typename Accessor>
  static constexpr auto indexOf = cg::indexOf<Accessor, Accessors...>();

  /// J-th accessor.
  template <size_t J>
  using Accessor = std::tuple_element_t<J, std::tuple<Accessors...>>;

}; // SlotCache

} // end namespace pa


/////////////////////////////////////////////////////////////////////
//
// PointIndexTraits: traits of a point index
// ================
template <typename I = int32_t, // point id (signed)
  typename P = pa::DefaultPosition, // position accessor
  typename C = pa::SlotCache<P>> // accessors of cached attributes
struct PointIndexTraits
{
  using PointId = I;
  using Position = P;
  using Cache = C;

}; // PointIndexTraits

template <typename I, bool cachePositions>
using DefaultPointIndexTraits = PointIndexTraits<I,
  pa::DefaultPosition,
  std::conditional_t<cachePositions,
    pa::SlotCache<pa::DefaultPosition>,
    pa::SlotCache<>>>;

// Forward definition
template <typename A, typename I, typename P, typename... Args>
class PointArray;

namespace pa
{ // begin namespace pa

/// Traits of the point indices of a PointArray: id type index_t,
/// position read by Field<0> and always cached, plus the fields J...
/// (indices of the attributes in the SoA, 1 for the first one
/// after the position). Usage:
///
/// using PA = PointArray<A, int, vec3f, vec3f, float>;
/// PointGrid<3, float, PA, pa::SoATraits<PA, 1, 2>> grid{...};
template <typename PA, size_t... J>
struct SoATraits;

template <typename A, typename I, typename P, typename... Args, size_t... J>
struct SoATraits<PointArray<A, I, P, Args...>, J...>:
  PointIndexTraits<I, Field<0>, SlotCache<Field<0>, Field<J>...>>
{
  static_assert(((J >= 1 && J <= sizeof...(Args)) && ...),
    "SoATraits: field index out of range");

}; // SoATraits

} // end namespace pa

} // end namespace cg

#endif // __PointIndexTraits_h
