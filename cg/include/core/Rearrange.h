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
// OVERVIEW: Rearrange.h
// ========
// Functions for rearrangement of collections in a given order.
//
// Author: Paulo Pagliosa
// Last revision: 10/10/2026

#ifndef __Rearrange_h
#define __Rearrange_h

#include <cassert>
#include <concepts>
#include <cstddef>
#include <span>
#include <utility>
#include <vector>

namespace cg
{ // begin namespace cg

template <typename T>
concept IsIntegral = std::integral<T>;

/// Returns true if order is a permutation of [0, order.size()),
/// i.e., a valid order for rearrange().
template <IsIntegral I>
[[nodiscard]] bool
isValidOrder(std::span<const I> order)
{
  const auto n = order.size();
  std::vector<bool> seen(n);

  for (auto i : order)
  {
    if (i < 0 || size_t(i) >= n || seen[size_t(i)])
      return false;
    seen[size_t(i)] = true;
  }
  return true;
}

/// Rearranges a swappable collection in the given order,
/// calling swap(i, j) to exchange its elements i and j (at most
/// order.size() - 1 times): afterwards, the element at k is the
/// one that was at order[k]. order must be a permutation of
/// [0, order.size()) (checked in debug builds); it is consumed:
/// it ends as the identity.
template <IsIntegral I, typename Swap>
void
rearrangeBySwap(std::span<I> order, Swap&& swap)
{
  assert(isValidOrder(std::span<const I>{order}));

  const auto n = I(order.size());

  // Each cycle of order is followed from its first position k.
  // Position x gets its element by swapping with the position
  // holding it (order[x]). The element displaced from k
  // travels along the cycle and lands at its last position,
  // which needs no swap. Setting order[x] = x marks x as done,
  // so order ends as the identity; positions already marked
  // are skipped.
   for (I k = 0; k < n; ++k)
  {
    if (order[k] == k)
      continue;

    auto x = k;

    for (auto next = order[x]; next != k; next = order[x])
    {
      swap(x, next);
      order[x] = x;
      x = next;
    }
    order[x] = x;
  }
}

template <typename C, typename I>
concept HasIndexSwap = requires (C& c, I i)
{
  c.swap(i, i);
  c.size();
};

/// Rearranges a collection with swap(i, j) and size() (e.g, SoA).
template <typename C, std::integral I>
  requires HasIndexSwap<C, I>
void
rearrange(C& c, std::span<I> order)
{
  assert(order.size() == size_t(c.size()));
  rearrangeBySwap(order, [&c](I i, I j)
  {
    c.swap(i, j);
  });
}

/// Rearranges an indexable collection with swappable elements and
/// size() (e.g., std::vector).
template <typename C, IsIntegral I>
  requires (!HasIndexSwap<C, I>) && requires (C& c, I i)
  {
    c.size();
    std::swap(c[i], c[i]);
  }
void
rearrange(C& c, std::span<I> order)
{
  assert(order.size() == size_t(c.size()));
  rearrangeBySwap(order, [&c](I i, I j)
  {
    std::swap(c[i], c[j]);
  });
}

} // end namespace cg

#endif // __Rearrange_h
