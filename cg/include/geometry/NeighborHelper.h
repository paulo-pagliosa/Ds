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
// OVERVIEW: NeighborHelper.h
// ========
// Types shared by the neighbor queries of point containers.
//
// Author: Paulo Pagliosa
// Last revision: 14/10/2026

#ifndef __NeighborHelper_h
#define __NeighborHelper_h

#include "core/Globals.h"
#include <concepts>
#include <cstddef>
#include <iterator>

namespace cg
{ // begin namespace cg

/// Range of slots [first, last) of a cell (grid) or node (tree).
/// A slot is a position in the arrays sorted by cell/node,
/// sortedIds() and sortedPositions(): the same slot gives the id
/// and the cached position of a point, and the points of a
/// cell/node occupy consecutive slots.
template <typename I>
struct SlotRange
{
  I first;
  I last;

  /// Iterator over the slots of a range (a plain counter).
  struct iterator
  {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = I;
    using pointer = void;
    using reference = I;

    I index{};

    [[nodiscard]] I operator *() const
    {
      return index;
    }

    iterator& operator ++()
    {
      ++index;
      return *this;
    }

    iterator operator ++(int)
    {
      auto temp = *this;

      ++index;
      return temp;
    }

    [[nodiscard]] bool operator ==(const iterator&) const = default;

  }; // iterator

  [[nodiscard]] auto begin() const
  {
    return iterator{first};
  }

  [[nodiscard]] auto end() const
  {
    return iterator{last};
  }

  [[nodiscard]] auto size() const
  {
    return size_t(last - first);
  }

  [[nodiscard]] bool empty() const
  {
    return first == last;
  }

}; // SlotRange

/// Argument of the functor of forEachNeighbor().
template <typename I, typename V>
struct Neighbor
{
  I id; ///< id of the neighbor (point q)
  typename V::value_type d2; ///< squared distance between p and q
  V d; ///< p - q

}; // Neighbor

/// Default norm of the kNN searches.
template <typename V>
struct SquaredNorm
{
  [[nodiscard]] HOST DEVICE
  auto operator ()(const V& v) const
  {
    return v.squaredNorm();
  }

}; // SquaredNorm

namespace internal::nh
{ // begin namespace internal::nh

template <typename F, typename Neighbor>
concept IsNeighborFunctor = std::is_invocable_v<F&, const Neighbor&> &&
std::is_same_v<std::invoke_result_t<F&, const Neighbor&>, bool>;

template <typename B, typename I>
concept IsIndexBuffer = requires (B & b, size_t i)
{
  { b.size() } -> std::convertible_to<size_t>;
  { b[i] } -> std::same_as<I&>;
};

} // end namespace internal::nh

} // end namespace cg

#endif // __NeighborHelper_h
