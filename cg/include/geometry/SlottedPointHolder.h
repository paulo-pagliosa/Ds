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
// OVERVIEW: SlottedPointHolder.h
// ========
// Class definition for slotted point holder.
//
// Author: Paulo Pagliosa
// Last revision: 23/10/2026

#ifndef __SlottedPointHolder_h
#define __SlottedPointHolder_h

#include "core/Array.h"
#include "geometry/IndexList.h"
#include "geometry/KNNResult.h"
#include "geometry/NeighborHelper.h"
#include "geometry/PointHolder.h"
#include <limits>
#include <stdexcept>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// SlottedPointHolder: slotted point holder class
// ==================
template <int D,
  IsReal R,
  typename PA,
  typename I = int32_t,
  bool cachePositions = true,
  typename Allocator = ArrayAllocator>
class SlottedPointHolder: public PointHolder<D, R, PA>
{
public:
  ASSERT_SIGNED(I, "PointHolder: signed int point id type expected");

  using Base = PointHolder<D, R, PA>;
  using PointId = I;
  using vec_type = Vector<R, D>;
  using allocator_type = Allocator;

  template <int K = 0> using KNNResult = cg::KNNResult<PointId, R, K>;
  using Neighbor = cg::Neighbor<PointId, vec_type>;
  using SquaredNorm = cg::SquaredNorm<vec_type>;
  using SlotRange = cg::SlotRange<PointId>;

  /// Raw view of the sorted arrays. A slot is a position in the
  /// sorted arrays: slot k holds the id of a point (ids[k]) and,
  /// if cached, a copy of its position (positions[k]).
  struct SlotArrays
  {
    const PointId* ids;
    const vec_type* positions; // nullptr if not cached
    PointId count;

  }; // SlotArrays

  ~SlottedPointHolder()
  {
    free();
  }

  /// Returns the number of points stored at the last update (the
  /// active points then).
  [[nodiscard]] auto pointCount() const
  {
    return _count;
  }

  /// Returns the number of points stored at the last update that
  /// were outside the domain of the container (which defines the
  /// domain and sets the count in its update).
  [[nodiscard]] auto outsideCount() const
  {
    return _outside;
  }

  [[nodiscard]] const auto* sortedIds() const
  {
    return _ids;
  }

  [[nodiscard]] const auto* sortedPositions() const
    requires cachePositions
  {
    return _positions;
  }

  [[nodiscard]] SlotArrays slotArrays() const
  {
    return {_ids, _positions, _count};
  }

  /// Rearranges the points so that each id becomes its slot: the
  /// active points get the ids [0, pointCount()), in slot order,
  /// and the inactive ones (removed points) the ids [pointCount(),
  /// points().size()), by rearrange(points(), order), with
  /// order[k] the id of the point that goes to k. Everything the
  /// owner keeps per point must be in the point set. The point set
  /// must not have changed since the last update (asserted in debug).
  /// No memory is allocated: order is the array of sorted ids,
  /// which ends as the identity; the container remains valid.
  /// Returns pointCount().
  PointId reorder()
    requires requires (PA& pa, std::span<PointId> order)
  {
    rearrange(pa, order);
  };

protected:
  explicit SlottedPointHolder(PA& points):
    Base{points}
  {
    // do nothing
  }

  SlottedPointHolder(const SlottedPointHolder&) = delete;
  SlottedPointHolder& operator =(const SlottedPointHolder&) = delete;

  /// A moved-from object can only be destroyed or assigned to.
  SlottedPointHolder(SlottedPointHolder&& other) noexcept:
    Base{other},
    _ids{std::exchange(other._ids, nullptr)},
    _positions{std::exchange(other._positions, nullptr)},
    _capacity{std::exchange(other._capacity, 0)},
    _count{std::exchange(other._count, 0)},
    _outside{std::exchange(other._outside, 0)}
  {
    // do nothing
  }

  SlottedPointHolder& operator =(SlottedPointHolder&& other) noexcept
  {
    if (this != &other)
    {
      free();
      Base::operator =(other);
      _ids = std::exchange(other._ids, nullptr);
      _positions = std::exchange(other._positions, nullptr);
      _capacity = std::exchange(other._capacity, 0);
      _count = std::exchange(other._count, 0);
      _outside = std::exchange(other._outside, 0);
    }
    return *this;
  }

  /// Returns the number of slots allocated.
  [[nodiscard]] auto slotCapacity() const
  {
    return _capacity;
  }

  /// Ensures the arrays have a slot for each point the point set can
  /// hold (its capacity(), or size() if it has none). If they must
  /// grow, the old arrays are freed before the new ones are allocated
  /// (no peak of memory), their contents are lost and the counts are
  /// reset. Returns true if the arrays were reallocated. Throws
  /// std::length_error if the capacity does not fit in PointId.
  bool fitCapacity();

  /// Mutable access to the sorted arrays (for the update of the derived
  /// class).
  [[nodiscard]] PointId* ids()
  {
    return _ids;
  }

  [[nodiscard]] vec_type* positions()
    requires cachePositions
  {
    return _positions;
  }

  /// Stores point i at slot k (and its position p, if cached).
  void setSlot(PointId k, PointId i, const vec_type& p)
  {
    assert(k >= 0 && size_t(k) < _capacity);
    _ids[k] = i;
    if constexpr (cachePositions)
      _positions[k] = p;
    else
      (void)p;
  }

  void setSlot(PointId k, PointId i)
  {
    setSlot(k, i, position(i));
  }

  /// Exchanges the contents of two slots.
  void swapSlots(PointId a, PointId b)
  {
    std::swap(_ids[a], _ids[b]);
    if constexpr (cachePositions)
      std::swap(_positions[a], _positions[b]);
  }

  /// Copies the positions of the points of the slots [first, last) to
  /// the cache (nothing if positions are not cached).
  void cacheSlotPositions(PointId first, PointId last)
  {
    if constexpr (cachePositions)
      for (auto k = first; k < last; ++k)
        _positions[k] = position(_ids[k]);
    else
      (void)first, (void)last;
  }

  /// Sets the counts at the end of an update.
  void setCounts(PointId count, PointId outside)
  {
    assert(count >= 0 && size_t(count) <= _capacity);
    assert(outside >= 0 && outside <= count);
    _count = count;
    _outside = outside;
  }

  /// Position of point i (from the point set).
  [[nodiscard]] auto position(PointId i) const
  {
    return vec_type{this->points()[i]};
  }

  /// Position of the point of slot k (from the cache, if any).
  [[nodiscard]] auto slotPosition(PointId k) const
  {
    if constexpr (cachePositions)
      return _positions[k];
    else
      return position(_ids[k]);
  }

  /// Returns a callable pos(k) -> position of the point of slot k, to be
  /// passed to the searches.
  [[nodiscard]] auto slotPositionOf() const
  {
    if constexpr (cachePositions)
      return [p = _positions](PointId k) -> const vec_type&
    {
      return p[k];
    };
    else
      return [this](PointId k)
    {
      return position(_ids[k]);
    };
  }

  /// Returns the number of active points of the point set.
  [[nodiscard]] PointId activePointCount() const;

  /// Returns the memory allocated by the sorted arrays, in bytes.
  [[nodiscard]] size_t slotMemoryUsage() const
  {
    return _capacity * (sizeof(PointId) + (cachePositions ? sizeof(vec_type) : 0));
  }

  /// Returns true if the slots [0, pointCount()) hold each active point
  /// exactly once, with its current position (if cached), and if
  /// slotOk(k, i) is true for every slot k and its point i (the check of
  /// the derived class, e.g., that the point is in the cell of the
  /// slot). For debugging: allocates and costs O(n).
  template <typename F>
  [[nodiscard]] bool isConsistent(F&& slotOk) const;

  [[nodiscard]] bool isConsistent() const
  {
    return isConsistent([](PointId, PointId) { return true; });
  }

private:
  PointId* _ids{}; // sorted point ids
  vec_type* _positions{}; // sorted point positions (if cached)
  size_t _capacity{}; // number of slots allocated
  PointId _count{}; // points stored at the last update
  PointId _outside{}; // points outside the domain

  void free() noexcept
  {
    Allocator::free(_ids);
    Allocator::free(_positions);
    _ids = nullptr;
    _positions = nullptr;
  }

}; // SlottedPointHolder

template <int D, IsReal R, typename PA, typename I, bool C, typename A>
bool
SlottedPointHolder<D, R, PA, I, C, A>::fitCapacity()
{
  const auto capacity = internal::ph::pointCapacity(this->points());

  if (capacity <= _capacity)
    return false;
  if (capacity > size_t(std::numeric_limits<PointId>::max()))
    throw std::length_error("SlottedPointHolder: too many points");
  // Free first: no peak of memory
  free();
  _capacity = 0;
  _count = _outside = 0;
  try
  {
    _ids = A::template allocate<PointId>(capacity);
    if constexpr (C)
      _positions = A::template allocate<vec_type>(capacity);
  }
  catch (...)
  {
    // Keep both arrays empty, so that the next call retries
    free();
    throw;
  }
  _capacity = capacity;
  return true;
}

template <int D, IsReal R, typename PA, typename I, bool C, typename A>
auto
SlottedPointHolder<D, R, PA, I, C, A>::activePointCount() const -> PointId
{
  const auto& points = this->points();
  const auto n = points.size();
  using psize_t = std::remove_const_t<decltype(n)>;
  PointId active{0};

  for (psize_t i = 0; i < n; ++i)
    active += this->isPointActive(i) ? 1 : 0;
  return active;
}

template <int D, IsReal R, typename PA, typename I, bool C, typename A>
template <typename F>
bool
SlottedPointHolder<D, R, PA, I, C, A>::isConsistent(F&& slotOk) const
{
  const auto n = size_t(this->points().size());

  if (activePointCount() != _count || size_t(_count) > _capacity)
    return false;

  std::vector<bool> seen(n);

  for (PointId k = 0; k < _count; ++k)
  {
    const auto i = _ids[k];

    if (i < 0 || size_t(i) >= n || seen[size_t(i)] || !this->isPointActive(i))
      return false;
    seen[size_t(i)] = true;
    if constexpr (C)
      if (!(_positions[k] == position(i)))
        return false;
    if (!slotOk(k, i))
      return false;
  }
  return true;
}

template <int D, IsReal R, typename PA, typename I, bool C, typename A>
auto
SlottedPointHolder<D, R, PA, I, C, A>::reorder()->PointId
  requires requires (PA& pa, std::span<PointId> order)
{
  rearrange(pa, order);
}
{
  auto& points = this->points();
  const auto n = PointId(points.size());

  if (n == 0)
    return 0;
  assert(size_t(n) <= _capacity);
  assert(activePointCount() == _count);
  // order[k] = id of the point that goes to slot (new id) k: the
  // sorted ids, then the inactive points
  for (PointId i = 0, k = _count; i < n; ++i)
    if (!this->isPointActive(i))
      _ids[k++] = i;
  rearrange(points, std::span<PointId>{_ids, size_t(n)});
  return _count;
}

} // end namespace cg

#endif // __SlottedPointHolder_h
