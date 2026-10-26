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
// Last revision: 26/10/2026

#ifndef __SlottedPointHolder_h
#define __SlottedPointHolder_h

#include "core/SoA.h"
#include "geometry/IndexList.h"
#include "geometry/KNNResult.h"
#include "geometry/NeighborHelper.h"
#include "geometry/PointHolder.h"
#include "geometry/PointIndexTraits.h"

namespace cg
{ // begin namespace cg

namespace pa::internal
{ // begin namespace pa::internal

template <typename PA>
concept HasCapacity = requires (const PA & points)
{
  points.capacity();
};

template <typename PA>
[[nodiscard]] inline auto
pointCapacity(const PA& points)
{
  if constexpr (HasCapacity<PA>)
    return size_t(points.capacity());
  else
    return size_t(points.size());
}

// SoA of the slots: ids, then the cached attributes
template <typename A, // allocator
  typename I, // point index
  typename V, // vector 
  typename PA, // point array
  typename P, // position accessor
  typename C> // accessors of cached attributes
struct SlotStorage;

template <typename A,
  typename I,
  typename V,
  typename PA,
  typename P,
  typename... Accessors>
struct SlotStorage<A, I, V, PA, P, SlotCache<Accessors...>>
{
  template <typename Accessor>
    requires cg::has<Accessor, Accessors...>
  using cached_type = std::conditional_t<std::is_same_v<Accessor, P>,
    V,
    attribute_type<Accessor, PA, I>>;

  using type = SoA<A, I, I, cached_type<Accessors>...>;

  static constexpr size_t bytesPerSlot = (sizeof(I) + ... +
    sizeof(cached_type<Accessors>));

}; // SlotStorage

} // end namespace pa::internal


/////////////////////////////////////////////////////////////////////
//
// SlottedPointHolder: slotted point holder class
// ==================
template <int D,
  IsReal R,
  typename PA,
  typename Traits = PointIndexTraits<>,
  typename Allocator = ArrayAllocator>
class SlottedPointHolder: public PointHolder<D, R, PA>
{
public:
  using PointId = typename Traits::PointId;

  ASSERT_SIGNED(PointId, "PointHolder: signed int point id type expected");

  using Base = PointHolder<D, R, PA>;
  using allocator_type = Allocator;
  using Position = typename Traits::Position;
  using Cache = typename Traits::Cache;
  using vec_type = Vector<R, D>;

  template <int K = 0, KNNKey key = KNNKey::Id>
  using KNNResult = cg::KNNResult<PointId, R, K, key>;

  using Neighbor = cg::Neighbor<PointId, vec_type>;
  using SquaredNorm = cg::SquaredNorm<vec_type>;
  using SlotRange = cg::SlotRange<PointId>;
  /// A slot as the center of a query (see NeighborHelper.h).
  using Slot = cg::Slot<PointId>;

  /// True if the positions are cached in the slots.
  static constexpr bool cachesPositions = Cache::template has<Position>;

  /// Number of cached attributes (including the position).
  static constexpr auto attributeCount = Cache::size;

  /// Type of the cached attribute J (the position is vec_type).
  template <size_t J>
  using attribute_type = typename pa::internal::SlotStorage<Allocator,
    PointId,
    vec_type,
    PA,
    Position,
    Cache>::template cached_type<typename Cache::template Accessor<J>>;

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
    return _slots.template data<0>();
  }

  /// Returns the array of the cached attribute J (J-th accessor
  /// of the cache), in slot order.
  template <size_t J>
    requires (J < attributeCount)
  [[nodiscard]] const auto* sortedAttribute() const
  {
    return _slots.template data<J + 1>();
  }

  /// Returns the array of the cached attribute read by Accessor,
  /// in slot order.
  template <typename Accessor>
    requires Cache::template has<Accessor>
  [[nodiscard]] const auto* sortedAttribute() const
  {
    return sortedAttribute<Cache::template indexOf<Accessor>>();
  }

  /// Returns the cached attribute J of the point of slot k.
  template <size_t J>
    requires (J < attributeCount)
  [[nodiscard]] const auto& slotAttribute(PointId k) const
  {
    assert(k >= 0 && k < _count);
    return sortedAttribute<J>()[k];
  }

  /// Returns the cached attribute read by Accessor of the point
  /// of slot k.
  template <typename Accessor>
    requires Cache::template has<Accessor>
  [[nodiscard]] const auto& slotAttribute(PointId k) const
  {
    return slotAttribute<Cache::template indexOf<Accessor>>(k);
  }

  [[nodiscard]] const auto* sortedPositions() const
    requires cachesPositions
  {
    return sortedAttribute<Position>();
  }

  /// Returns the bounds (a cube, if squared)of the active points
  /// of a point set, read with the position accessor.
  [[nodiscard]] static auto computeBounds(const PA& points,
    bool squared = false)
  {
    return Base::computeBounds(points, squared, Position{});
  }

  /// Rearranges the points so that each id matches its slot: the
  /// active points get the ids [0, pointCount()), in slot order,
  /// and the inactive, removed points get the ids [pointCount(),
  /// points().size()), by calling rearrange(points(), order),
  /// where order[k] is the id of the point that goes into slot k,
  /// and then compacts itself, discarding the removed points.
  /// All data the owner maintains per point must be stored in the
  /// point set, which must not have changed since the last update
  /// (asserted in debug builds). No memory is allocated: order is
  /// the array of sorted ids, which ends up as the identity
  /// permutation. Returns pointCount().
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
    _slots{std::move(other._slots)},
    _count{std::exchange(other._count, 0)},
    _outside{std::exchange(other._outside, 0)}
  {
    // do nothing
  }

  SlottedPointHolder& operator =(SlottedPointHolder&& other) noexcept
  {
    if (this != &other)
    {
      Base::operator =(other);
      _slots = std::move(other._slots);
      _count = std::exchange(other._count, 0);
      _outside = std::exchange(other._outside, 0);
    }
    return *this;
  }

  /// Returns the number of slots allocated.
  [[nodiscard]] auto slotCapacity() const
  {
    return size_t(_slots.size());
  }

  /// Ensures the arrays have a slot for each point the point set
  /// can hold. Returns true if the arrays were reallocated.
  bool fitCapacity();

  /// Mutable access to the sorted ids (for derived classes).
  [[nodiscard]] PointId* ids()
  {
    return _slots.template data<0>();
  }

  /// Stores point i at slot k and copies its cached attributes; p
  /// is its position (computed by the caller).
  void setSlot(PointId k, PointId i, const vec_type& p)
  {
    assert(k >= 0 && size_t(k) < slotCapacity());
    ids()[k] = i;
    gather(k, i, &p, Indices{});
  }

  /// Stores point i at slot k and copies its cached attributes.
  void setSlot(PointId k, PointId i)
  {
    assert(k >= 0 && size_t(k) < slotCapacity());
    ids()[k] = i;
    gather(k, i, nullptr, Indices{});
  }

  /// Exchanges the contents of two slots.
  void swapSlots(PointId a, PointId b)
  {
    _slots.swap(a, b);
  }

  /// Copies the cached attributes of the points of the slots
  /// [first, last), whose ids are set, from the point set.
  void gatherSlots(PointId first, PointId last)
  {
    if constexpr (attributeCount > 0)
    {
      const auto ids = sortedIds();

      for (auto k = first; k < last; ++k)
        gather(k, ids[k], nullptr, Indices{});
    }
    else
      (void)first, (void)last;
  }

  /// Sets the counts at the end of an update.
  void setCounts(PointId count, PointId outside)
  {
    assert(count >= 0 && size_t(count) <= slotCapacity());
    assert(outside >= 0 && outside <= count);
    _count = count;
    _outside = outside;
  }

  /// Position of point i (from the point set).
  [[nodiscard]] auto position(PointId i) const
  {
    return vec_type{Position{}(this->points(), i)};
  }

  /// Position of the point of slot k (from the cache, if any).
  [[nodiscard]] auto slotPosition(PointId k) const
  {
    if constexpr (cachesPositions)
      return sortedPositions()[k];
    else
      return position(sortedIds()[k]);
  }

  /// Returns a callable pos(k) -> position of the point of slot
  /// k, to be passed to a search method.
  [[nodiscard]] auto slotPositionOf() const
  {
    if constexpr (cachesPositions)
      return [p = sortedPositions()](PointId k) -> const vec_type&
    {
      return p[k];
    };
    else
      return [this](PointId k)
    {
      return position(sortedIds()[k]);
    };
  }

  /// Returns the number of active points of the point set.
  [[nodiscard]] PointId activePointCount() const;

  /// Returns the memory allocated by the sorted arrays, in bytes.
  [[nodiscard]] size_t slotMemoryUsage() const
  {
    return slotCapacity() * Storage::bytesPerSlot;
  }

  /// Returns true if the slots [0, pointCount()) hold each active
  /// point exactly once, and if f(k, i) is true for every slot k
  /// and its point i. For debugging: allocates and costs O(n).
  template <typename F>
  [[nodiscard]] bool isConsistent(F&& f) const;

  [[nodiscard]] bool isConsistent() const
  {
    return isConsistent([](PointId, PointId)
    {
      return true;
    });
  }

private:
  using Storage = pa::internal::SlotStorage<Allocator,
    PointId,
    vec_type,
    PA,
    Position,
    Cache>;
  using Indices = std::make_index_sequence<attributeCount>;

  typename Storage::type _slots; // ids and cached attributes
  PointId _count{}; // points stored at the last update
  PointId _outside{}; // points outside the domain

  // Value of the cached attribute J of point i (p: its position,
  // if known).
  template <size_t J>
  auto attribute(PointId i, const vec_type* p) const
  {
    using Accessor = typename Cache::template Accessor<J>;

    if constexpr (std::is_same_v<Accessor, Position>)
      return p ? *p : position(i);
    else
      return attribute_type<J>(Accessor{}(this->points(), i));
  }

  template <size_t... J>
  void gather(PointId k, PointId i,
    const vec_type* p,
    std::index_sequence<J...>)
  {
    ((_slots.template data<J + 1>()[k] = attribute<J>(i, p)), ...);
    (void)i, (void)p;
  }

  template <size_t... J>
  bool sameAttributes(PointId k, PointId i, std::index_sequence<J...>) const
  {
    auto same = [&]<size_t L>(std::integral_constant<size_t, L>)
    {
      using T = attribute_type<L>;

      if constexpr (std::equality_comparable<T>)
        return bool(sortedAttribute<L>()[k] == attribute<L>(i, nullptr));
      else
        return true;
    };

    (void)k, (void)i;
    return (same(std::integral_constant<size_t, J>{}) && ...);
  }

}; // SlottedPointHolder

template <int D, IsReal R, typename PA, typename T, typename A>
bool
SlottedPointHolder<D, R, PA, T, A>::fitCapacity()
{
  const auto capacity = pa::internal::pointCapacity(this->points());

  if (capacity <= slotCapacity())
    return false;
  if (capacity > size_t(std::numeric_limits<PointId>::max()))
    throw std::length_error("PointHolder: too many points");
  _count = _outside = 0;
  // SoA::reallocate() frees first (no peak of memory) and leaves
  // the arrays empty if the allocation fails.
  _slots.reallocate(PointId(capacity));
  return true;
}

template <int D, IsReal R, typename PA, typename T, typename A>
auto
SlottedPointHolder<D, R, PA, T, A>::activePointCount() const -> PointId
{
  const auto& points = this->points();
  const auto n = points.size();
  using psize_t = std::remove_const_t<decltype(n)>;
  PointId active{0};

  for (psize_t i = 0; i < n; ++i)
    active += this->isPointActive(i) ? 1 : 0;
  return active;
}

template <int D, IsReal R, typename PA, typename T, typename A>
template <typename F>
bool
SlottedPointHolder<D, R, PA, T, A>::isConsistent(F&& slotOk) const
{
  const auto n = size_t(this->points().size());

  if (activePointCount() != _count || size_t(_count) > slotCapacity())
    return false;

  const auto ids = sortedIds();
  std::vector<bool> seen(n);

  for (PointId k = 0; k < _count; ++k)
  {
    const auto i = ids[k];

    if (i < 0 || size_t(i) >= n || seen[size_t(i)] || !this->isPointActive(i))
      return false;
    seen[size_t(i)] = true;
    if (!sameAttributes(k, i, Indices{}))
      return false;
    if (!slotOk(k, i))
      return false;
  }
  return true;
}

template <int D, IsReal R, typename PA, typename T, typename A>
auto
SlottedPointHolder<D, R, PA, T, A>::reorder()->PointId
  requires requires (PA& pa, std::span<PointId> order)
{
  rearrange(pa, order);
}
{
  auto& points = this->points();
  const auto n = PointId(points.size());

  if (n == 0)
    return 0;
  assert(size_t(n) <= slotCapacity());
  assert(activePointCount() == _count);

  const auto ids = this->ids();

  // order[k] = id of the point that goes to slot (new id) k: the
  // sorted ids, then the inactive points.
  for (PointId i = 0, k = _count; i < n; ++i)
    if (!this->isPointActive(i))
      ids[k++] = i;
  rearrange(points, std::span<PointId>{ids, size_t(n)});
  return _count;
}

} // end namespace cg

#endif // __SlottedPointHolder_h
