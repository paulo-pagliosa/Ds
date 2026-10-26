//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2016, 2026 Paulo Pagliosa.                        |
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
// OVERVIEW: PointGridBase.h
// ========
// Class definition for point grid base.
//
// Author: Paulo Pagliosa
// Last revision: 26/10/2026

#ifndef __PointGridBase_h
#define __PointGridBase_h

#include "geometry/GridBase.h"
#include "geometry/SlottedPointHolder.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// PointGrid: generic point grid class
// =========
template <int D,
  IsReal R,
  typename PA,
  typename Traits = PointIndexTraits<>,
  typename Allocator = ArrayAllocator>
class PointGrid:
  public RegionGrid<D, R, typename Traits::PointId, Allocator>,
  public SlottedPointHolder<D, R, PA, Traits, Allocator>
{
public:
  using Base = RegionGrid<D, R, typename Traits::PointId, Allocator>;
  using Points = SlottedPointHolder<D, R, PA, Traits, Allocator>;
  using grid_type = PointGrid<D, R, PA, Traits, Allocator>;
  using id_type = typename Base::id_type;
  using index_type = typename Base::index_type;
  using bounds_type = typename Base::bounds_type;
  using vec_type = typename Base::vec_type;
  using PointId = typename Points::PointId;
  using allocator_type = Allocator;

  template <int K = 0, KNNKey key = KNNKey::Id>
  using KNNResult = typename Points::template KNNResult<K, key>;

  using Neighbor = typename Points::Neighbor;
  using SquaredNorm = typename Points::SquaredNorm;
  using SlotRange = typename Points::SlotRange;
  using Slot = typename Points::Slot;

  /// Constructs a grid with cells of size h over domain for points.
  PointGrid(const bounds_type& domain, PA& points, R h):
    Base{domain, h},
    Points{points}
  {
    update();
  }

  /// Constructs a grid with size cells over domain for points.
  PointGrid(const bounds_type& domain, PA& points, const index_type& size):
    Base{domain, size},
    Points{points}
  {
    update();
  }

  /// Constructs a grid with cells of size h over the bounds of
  /// points (a cube, if squared).
  PointGrid(PA& points, R h, bool squared = true):
    PointGrid{Points::computeBounds(points, squared), points, h}
  {
    // do nothing
  }

  PointGrid(const PointGrid&) = delete;
  PointGrid& operator =(const PointGrid&) = delete;

  /// A moved-from grid can only be destroyed or assigned to.
  PointGrid(PointGrid&& other) noexcept:
    Base{std::move(other)},
    Points{std::move(other)}
  {
    // do nothing
  }

  /// Notifies the grid that the point set changed (points moved,
  /// added or removed) and rebuilds it from the current positions.
  /// Queries reflect the state of the last call. No memory is
  /// allocated, unless the capacity of the point set grew since
  /// the last call.
  void update();

  /// Rearranges the points so that each id matches its slot: the
  /// active points get the ids [0, pointCount()), in the order of
  /// sortedIds() (by cell), and the inactive, removed points get
  /// the ids [pointCount(), points().size()), by rearrange(points(),
  /// order), with order[k] the id of the point that goes into slot
  /// k, and then compacts itself, discarding the removed points.
  /// Everything the owner keeps per point must be in the point set.
  /// No memory is allocated: order is the array of sorted ids, which
  /// ends up as the identity permutation.
  PointId reorder()
    requires requires (PA& pa, std::span<PointId> order)
  {
    rearrange(pa, order);
  }
  {
    assert(isConsistent());
    return Points::reorder();
  }

  /// Returns the approximate memory used by the grid, in bytes.
  [[nodiscard]] auto memoryUsage() const
  {
    return size_t(this->length()) * sizeof(PointId) + this->slotMemoryUsage();
  }

  /// Iterator over the cells of the grid (in cell id order), whose
  /// value is the SlotRange of the cell; id() and index() give the
  /// cell, as in the iterators of Grid.
  class CellIterator
  {
  public:
    using iterator_concept = std::forward_iterator_tag;
    using iterator_category = std::input_iterator_tag;
    using value_type = SlotRange;
    using difference_type = std::ptrdiff_t;
    using pointer = void;
    using reference = SlotRange;

    CellIterator() = default;

    CellIterator& operator ++()
    {
      ++_id;
      return *this;
    }

    CellIterator operator ++(int)
    {
      auto temp = *this;

      ++_id;
      return temp;
    }

    [[nodiscard]] bool operator ==(const CellIterator& other) const
    {
      assert(_grid == other._grid);
      return _id == other._id;
    }

    [[nodiscard]] auto operator *() const
    {
      return _grid->cell(_id);
    }

    [[nodiscard]] auto id() const
    {
      return _id;
    }

    [[nodiscard]] auto index() const
    {
      return _grid->cellIndex(_id);
    }

  private:
    const grid_type* _grid{};
    id_type _id{};

    CellIterator(const grid_type* grid, id_type id):
      _grid{grid},
      _id{id}
    {
      // do nothing
    }

    friend grid_type;

  }; // CellIterator

  using iterator = CellIterator;
  using const_iterator = CellIterator;

  /// Returns the range of slots of a cell (see SlotRange). The grid
  /// is a container of cells: operator [] is a synonym.
  /// @note: the argument is a cell id (or index, or a point of the
  /// domain), not a point id.
  [[nodiscard]] auto cell(id_type id) const
  {
    return SlotRange{cellBegin(id), cellEnd(id)};
  }

  [[nodiscard]] auto cell(const index_type& index) const
  {
    return cell(Base::cellId(index));
  }

  /// Cell containing p, which must be in the domain.
  [[nodiscard]] auto cell(const vec_type& p) const
  {
    return cell(Base::cellId(p));
  }

  [[nodiscard]] auto operator [](id_type id) const
  {
    return cell(id);
  }

  [[nodiscard]] auto operator [](const index_type& index) const
  {
    return cell(index);
  }

  [[nodiscard]] auto operator [](const vec_type& p) const
  {
    return cell(p);
  }

  [[nodiscard]] auto begin() const
  {
    return CellIterator{this, 0};
  }

  [[nodiscard]] auto end() const
  {
    return CellIterator{this, this->length()};
  }

  [[nodiscard]] auto cbegin() const
  {
    return begin();
  }

  [[nodiscard]] auto cend() const
  {
    return end();
  }

  /// Raw arrays (e.g., to upload to the GPU).
  [[nodiscard]] const auto* cellStart() const
  {
    return Base::cells();
  }

  /// Calls f(const Neighbor&) for every point q in the grid with
  /// |p - q| <= radius, in no particular order. f returns true to
  /// continue the search or false to stop it. Returns the number
  /// of calls to f. No memory is allocated.
  template <typename F>
  size_t forEachNeighbor(const vec_type& p, R radius, F&& f) const
  {
    return radiusSearch(p, radius, PointId(-1), f);
  }

  /// Same as above, centered at point i and excluding i itself
  /// (by id: other points coincident with i are reported).
  template <typename F>
  size_t forEachNeighbor(PointId i, R radius, F&& f) const
  {
    assert(i >= 0 && size_t(i) < size_t(this->points().size()));
    return radiusSearch(this->position(i), radius, i, f);
  }

  /// Same as above, centered at the point of slot s.k.
  template <typename F>
  size_t forEachNeighbor(Slot s, R radius, F&& f) const
  {
    assert(s.k >= 0 && s.k < this->pointCount());
    return radiusSearch(this->slotPosition(s.k),
      radius,
      this->sortedIds()[s.k],
      f);
  }

  /// Collects the ids of the points q with |p - q| <= radius, in
  /// no particular order, into out, a buffer of FIXED size: any
  /// type with size() and operator[] returning PointId& (std::span,
  /// std::array, std::vector, ...). The capacity is out.size():
  /// the buffer is neither resized nor allocated. Returns the total
  /// number n of neighbors found: only out[0, min(n, out.size()))
  /// is meaningful, and n > out.size() means the result was
  /// truncated. (The other slots may be overwritten.) To collect
  /// into a growable container, use forEachNeighbor() instead.
  /// Faster than collecting with forEachNeighbor(): the candidates
  /// are written without branching on the distance test, which
  /// avoids a branch misprediction per neighbor.
  template <typename B>
  [[nodiscard]] size_t collectNeighbors(const vec_type& p,
    R radius,
    B& out) const
  {
    return collect<false>(p, radius, PointId(-1), out);
  }

  /// Same as above, centered at point i and excluding i itself.
  template <typename B>
  [[nodiscard]] size_t collectNeighbors(PointId i, R radius, B& out) const
  {
    assert(i >= 0 && size_t(i) < size_t(this->points().size()));
    return collect<false>(this->position(i), radius, i, out);
  }

  /// Same as above, centered at the point of slot s.k.
  template <typename B>
  [[nodiscard]] size_t collectNeighbors(Slot s, R radius, B& out) const
  {
    assert(s.k >= 0 && s.k < this->pointCount());
    return collect<false>(this->slotPosition(s.k),
      radius,
      this->sortedIds()[s.k],
      out);
  }

  /// Same as collectNeighbors(), but collects the SLOTS of the
  /// neighbors (to read cached attributes, or results kept in slot
  /// order, sequentially); the point ids are sortedIds()[slot].
  template <typename B>
  [[nodiscard]] size_t collectSlots(const vec_type& p, R radius, B& out) const
  {
    return collect<true>(p, radius, PointId(-1), out);
  }

  template <typename B>
  [[nodiscard]] size_t collectSlots(PointId i, R radius, B& out) const
  {
    assert(i >= 0 && size_t(i) < size_t(this->points().size()));
    return collect<true>(this->position(i), radius, i, out);
  }

  template <typename B>
  [[nodiscard]] size_t collectSlots(Slot s, R radius, B& out) const
  {
    assert(s.k >= 0 && s.k < this->pointCount());
    return collect<true>(this->slotPosition(s.k),
      radius,
      this->sortedIds()[s.k],
      out);
  }

  /// Finds the knn.k() nearest points to p whose distance (norm
  /// value) is <= knn.maxDistance(). The id and/or slot of the
  /// nearest points are stored in knn in ascending order of
  /// distance. Returns knn.size(). norm(const vec_type&) can be
  /// any callable that is monotone with respect to the absolute
  /// value of each point coordinate (e.g., an Lp norm, weighted
  /// or not, or its square). The default norm is he squared
  /// Euclidean distance. No memory is allocated.
  template <int K, KNNKey key, typename Norm = SquaredNorm>
  int findNearestNeighbors(const vec_type& p,
    KNNResult<K, key>& knn,
    Norm norm = Norm{}) const
  {
    return nearestSearch(p, PointId(-1), knn, norm);
  }

  /// Same as above, for point i and excluding i itself (by id).
  template <int K, KNNKey key, typename Norm = SquaredNorm>
  int findNearestNeighbors(PointId i,
    KNNResult<K, key>& knn,
    Norm norm = Norm{}) const
  {
    assert(i >= 0 && size_t(i) < size_t(this->points().size()));
    return nearestSearch(this->position(i), i, knn, norm);
  }

  /// Same as above, for the point of slot s.k and excluding it.
  template <int K, KNNKey key, typename Norm = SquaredNorm>
  int findNearestNeighbors(Slot s,
    KNNResult<K, key>& knn,
    Norm norm = Norm{}) const
  {
    assert(s.k >= 0 && s.k < this->pointCount());
    return nearestSearch(this->slotPosition(s.k),
      this->sortedIds()[s.k],
      knn,
      norm);
  }

private:
  // Checks the sorted arrays (see Points::isConsistent()) and that
  // every point is in the slots of its cell (for debugging).
  bool isConsistent() const;

  // Cell of p, clamped to the grid; inside is set to contains(p)
  // (computed together, on the same floatIndex(p)).
  id_type bucket(const vec_type& p, bool& inside) const;

  auto cellBegin(id_type id) const
  {
    return Base::cells()[id];
  }

  auto cellEnd(id_type id) const
  {
    return id + 1 < this->length() ? Base::cells()[id + 1] :
      this->pointCount();
  }

  // Calls f(first, last) with the slot range of the cells of the
  // row c.y (for D = 2, or c.z for D = 3) from x0 to x1: the cells
  // of a row are consecutive, so are their slots. f returns false
  // to stop the visit.
  template <typename F>
  bool visitRow(index_type c, id_type x0, id_type x1, F& f) const
  {
    const auto id = (c.x = x0, Base::cellId(c));
    return f(cellBegin(id), cellEnd(id + (x1 - x0)));
  }

  // Visits the rows of the cells in [lo, hi] (both valid indices).
  template <typename F>
  bool visitBox(const index_type& lo, const index_type& hi, F& f) const;

  // Visits the cells (in rows) at Chebyshev distance ring from c,
  // clipped to the grid
  template <typename F>
  bool visitShell(const index_type& c, id_type ring, F& f) const;

  template <typename F>
  size_t radiusSearch(const vec_type& p,
    R radius,
    PointId self,
    F& f) const;

  // Collects the ids (or the slots, if slots) of the neighbors.
  template <bool slots, typename B>
  size_t collect(const vec_type& p,
    R radius,
    PointId self,
    B& out) const;

  template <int K, KNNKey key, typename N>
  int nearestSearch(const vec_type& p,
    PointId self,
    KNNResult<K, key>& knn,
    N& norm) const;

}; // PointGrid

template <int D, IsReal R, typename PA, typename T, typename A>
inline auto
PointGrid<D, R, PA, T, A>::bucket(const vec_type& p,
  bool& inside) const -> id_type
{
  const auto f = this->floatIndex(p);
  const auto& n = this->size();
  index_type c;

  inside = true;
  for (int k = 0; k < D; ++k)
  {
    const bool ge = f[k] >= 0; // false for NaN
    const auto v = ge ? f[k] : R(0);
    const auto t = v < Base::maxIndex ? id_type(v) : n[k];

    inside = inside && ge && t < n[k];
    c[k] = t < n[k] ? t : n[k] - 1;
  }
  return Base::cellId(c);
}

template <int D, IsReal R, typename PA, typename T, typename A>
void
PointGrid<D, R, PA, T, A>::update()
{
  const auto& points = this->points();

  this->fitCapacity();

  const auto n = points.size();
  using psize_t = std::remove_const_t<decltype(n)>;

  assert(size_t(n) <= this->slotCapacity());

  const auto start = Base::cells();
  const auto length = this->length();
  bool inside;

  // 1. Count the points of each cell (and those outside bounds()).
  std::fill_n(start, length, PointId(0));

  PointId count{0};
  PointId outside{0};

  for (psize_t i = 0; i < n; ++i)
    if (this->isPointActive(i))
    {
      ++start[bucket(this->position(PointId(i)), inside)];
      ++count;
      outside += !inside;
    }
  // 2. Inclusive prefix sum: start[c] = end slot of cell c.
  {
    PointId sum{0};

    for (id_type c = 0; c < length; ++c)
      start[c] = (sum += start[c]);
  }
  // 3. Scatter backwards: decrementing the end slots leaves
  // start[c] as the first slot of cell c and keeps ids ascending
  // in the cell.
  for (psize_t i = n; i-- > 0;)
    if (this->isPointActive(i))
    {
      const auto p = this->position(PointId(i));
      // Same cell as bucket(p) in pass 1 (same computation).
      const auto k = --start[this->clampedCellId(p)];

      this->setSlot(k, PointId(i), p);
    }
  this->setCounts(count, outside);
}

template <int D, IsReal R, typename PA, typename T, typename A>
bool
PointGrid<D, R, PA, T, A>::isConsistent() const
{
  if (!Points::isConsistent())
    return false;

  const auto ids = this->sortedIds();
  PointId end{0};

  // The cells cover the slots [0, pointCount()) in order, and every
  // point is in the slots of its cell.
  for (id_type c = 0, length = this->length(); c < length; ++c)
  {
    if (cellBegin(c) != end || cellEnd(c) < end)
      return false;
    for (auto k = cellBegin(c), e = cellEnd(c); k < e; ++k)
      if (this->clampedCellId(this->position(ids[k])) != c)
        return false;
    end = cellEnd(c);
  }
  return end == this->pointCount();
}

template <int D, IsReal R, typename PA, typename T, typename A>
template <typename F>
inline bool
PointGrid<D, R, PA, T, A>::visitBox(const index_type& lo,
  const index_type& hi,
  F& f) const
{
  auto c = lo;

  if constexpr (D == 2)
  {
    for (c.y = lo.y; c.y <= hi.y; ++c.y)
      if (!visitRow(c, lo.x, hi.x, f))
        return false;
  }
  else
  {
    for (c.z = lo.z; c.z <= hi.z; ++c.z)
      for (c.y = lo.y; c.y <= hi.y; ++c.y)
        if (!visitRow(c, lo.x, hi.x, f))
          return false;
  }
  return true;
}

template <int D, IsReal R, typename PA, typename T, typename A>
template <typename F>
bool
PointGrid<D, R, PA, T, A>::visitShell(const index_type& c,
  id_type ring,
  F& f) const
{
  const auto& n = this->size();
  const auto x0 = math::max(c.x - ring, id_type(0));
  const auto x1 = math::min(c.x + ring, n.x - 1);
  const auto y0 = math::max(c.y - ring, id_type(0));
  const auto y1 = math::min(c.y + ring, n.y - 1);
  // Rows on the faces of the shell are visited entirely; the other
  // rows only have their two end cells on the shell.
  auto row = [&](index_type r, bool face)
  {
    if (face)
      return visitRow(r, x0, x1, f);
    if (c.x - ring >= 0 && !visitRow(r, c.x - ring, c.x - ring, f))
      return false;
    return c.x + ring >= n.x || visitRow(r, c.x + ring, c.x + ring, f);
  };
  auto r = c;

  if constexpr (D == 2)
  {
    for (r.y = y0; r.y <= y1; ++r.y)
      if (!row(r, ring == 0 || r.y == c.y - ring || r.y == c.y + ring))
        return false;
  }
  else
  {
    const auto z0 = math::max(c.z - ring, id_type(0));
    const auto z1 = math::min(c.z + ring, n.z - 1);

    for (r.z = z0; r.z <= z1; ++r.z)
    {
      const auto zFace = ring == 0 || r.z == c.z - ring || r.z == c.z + ring;

      for (r.y = y0; r.y <= y1; ++r.y)
        if (!row(r, zFace || r.y == c.y - ring || r.y == c.y + ring))
          return false;
    }
  }
  return true;
}

template <int D, IsReal R, typename PA, typename T, typename A>
template <typename F>
size_t
PointGrid<D, R, PA, T, A>::radiusSearch(const vec_type& p,
  R radius,
  PointId self,
  F& f) const
{
  static_assert(internal::nh::IsNeighborFunctor<F, Neighbor>,
    "PointGrid: functor must be bool(const Neighbor&) "
    "(return true to continue the search, false to stop it)");

  if (!(radius >= 0) || this->pointCount() == 0) // also rejects NaN
    return 0;

  const auto r2 = radius * radius;
  const auto ids = this->sortedIds();
  const vec_type r{radius};
  size_t calls = 0;
  auto test = [&](PointId first, PointId last)
  {
    for (auto k = first; k < last; ++k)
    {
      Neighbor nb;

      nb.d = p - this->slotPosition(k);
      nb.d2 = nb.d.squaredNorm();
      if (nb.d2 <= r2)
      {
        nb.id = ids[k];
        nb.slot = k;
        if (nb.id == self)
          continue;
        ++calls;
        if (!f(std::as_const(nb)))
          return false;
      }
    }
    return true;
  };

  visitBox(this->clampedCellIndex(p - r), this->clampedCellIndex(p + r), test);
  return calls;
}

template <int D, IsReal R, typename PA, typename T, typename A>
template <bool slots, typename B>
size_t
PointGrid<D, R, PA, T, A>::collect(const vec_type& p,
  R radius,
  PointId self,
  B& out) const
{
  static_assert(internal::nh::IsIndexBuffer<B, PointId>,
    "PointGrid: buffer must have size() and operator[] returning PointId&");

  if (!(radius >= 0) || this->pointCount() == 0) // also rejects NaN
    return 0;

  const auto r2 = radius * radius;
  const auto ids = this->sortedIds();
  const auto capacity = size_t(out.size());
  const vec_type r{radius};
  size_t n = 0; // neighbors found (written while n < capacity)
  auto distance = [this](const vec_type& p, PointId k)
  {
    return (p - this->slotPosition(k)).squaredNorm();
  };
  auto test = [&](PointId first, PointId last)
  {
    auto k = first;

    // Branchless: every candidate is written to out[n] and n
    // advances only if it passes; a rejected one is overwritten by
    // the next.
    for (; k < last && n < capacity; ++k)
    {
      const auto id = ids[k];

      out[n] = slots ? k : id;
      n += size_t((distance(p, k) <= r2) & (id != self));
    }
    // Buffer full: only counts
    for (; k < last; ++k)
      n += size_t((distance(p, k) <= r2) & (ids[k] != self));
    return true;
  };

  visitBox(this->clampedCellIndex(p - r), this->clampedCellIndex(p + r), test);
  return n;
}

template <int D, IsReal R, typename PA, typename T, typename A>
template <int K, KNNKey key, typename N>
int
PointGrid<D, R, PA, T, A>::nearestSearch(const vec_type& p,
  PointId self,
  KNNResult<K, key>& knn,
  N& norm) const
{
  static_assert(std::is_invocable_r_v<R, N&, const vec_type&>,
    "PointGrid: norm must be callable as R(const vec_type&)");

  knn.reset();
  if (this->pointCount() == 0)
    return 0;

  const auto ids = this->sortedIds();
  auto test = [&](PointId first, PointId last)
  {
    for (auto s = first; s < last; ++s)
      if (const auto d = R(norm(p - this->slotPosition(s))); d <= knn.limit())
        if (const auto id = ids[s]; id != self)
          knn.insert(id, s, d);
    return true;
  };

  const auto c = this->clampedCellIndex(p);
  const auto& n = this->size();
  const auto& o = this->bounds().min();
  const auto h = this->cellSize();
  id_type maxRing = 0;

  for (int a = 0; a < D; ++a)
    maxRing = math::max(maxRing, math::max(c[a], n[a] - 1 - c[a]));
  for (id_type ring = 0; ring <= maxRing; ++ring)
  {
    visitShell(c, ring, test);

    // Any point not visited yet is in a cell outside the box of
    // cells [c - ring, c + ring] (points outside the bounds are in
    // border cells, beyond the outer faces of the box), so it is at
    // least gap away from p along the axis a of some face of the
    // box that is not on the border. By the monotonicity of the
    // norm, its distance is at least norm(gap * e_a).
    auto bound = KNNResult<K, key>::infinity();
    auto faceBound = [&](int a, R gap)
    {
      vec_type g{R(0)};

      g[a] = math::max(gap, R(0));
      bound = math::min(bound, R(norm(g)));
    };

    for (int a = 0; a < D; ++a)
    {
      if (c[a] - ring > 0)
        faceBound(a, p[a] - (o[a] + R(c[a] - ring) * h[a]));
      if (c[a] + ring < n[a] - 1)
        faceBound(a, o[a] + R(c[a] + ring + 1) * h[a] - p[a]);
    }
    if (!knn.accepts(bound))
      break; // no unvisited point can be kept
    // (if the whole grid has been visited, the loop ends by itself)
  }
  knn.finish();
  return knn.size();
}

} // namespace cg

#endif // __PointGridBase_h
