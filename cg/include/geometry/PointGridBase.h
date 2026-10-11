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
// Last revision: 12/10/2026

#ifndef __PointGridBase_h
#define __PointGridBase_h

#include "geometry/GridBase.h"
#include "geometry/IndexList.h"
#include "geometry/KNNHelper.h"
#include "geometry/PointHolder.h"
#include "geometry/KNearest.h"

namespace cg
{ // begin namespace cg

namespace internal::pg
{ // begin namespace internal::pg

template <typename PA>
concept hasCapacity = requires (const PA& points)
{
  points.capacity();
};

template <typename PA>
[[nodiscard]] inline auto
pointCapacity(const PA& points)
{
  if constexpr (hasCapacity<PA>)
    return size_t(points.capacity());
  else
    return size_t(points.size());
}

} // end namespace internal::pg


/////////////////////////////////////////////////////////////////////
//
// PointGrid: generic point tree class
// =========
template <int D,
  IsReal R,
  typename PA,
  typename I = int,
  bool cachePositions = true>
class PointGrid: public RegionGrid<D, R, I>,
  public PointHolder<D, R, PA>
{
public:
  ASSERT_SIGNED(I, "PointGrid: signed int point id type expected");

  using Base = RegionGrid<D, R, I>;
  using grid_type = PointGrid<D, R, PA, I, cachePositions>;
  using vec_type = typename Base::vec_type;
  using bounds_type = typename Base::bounds_type;
  using index_type = typename Base::index_type;
  using id_type = typename Base::id_type;
  using PointSet = PointHolder<D, R, PA>;
  using PointId = I;

  /// Result type of findNearestNeighbors(): KNearest<PointId, R, K>
  /// with K = 0 (k given at run time) or K > 0 (fixed, no allocation).
  template <int K = 0>
  using knn_type = KNearest<PointId, R, K>;

  /// Argument of the functor of forEachNeighbor().
  struct Neighbor
  {
    PointId id; ///< id of the neighbor (point q)
    R d2; ///< squared distance between p and q
    vec_type d; ///< p - q

  }; // Neighbor

  /// Default norm of the kNN search (squared Euclidean distance).
  struct SquaredNorm
  {
    R operator ()(const vec_type& v) const
    {
      return v.squaredNorm();
    }

  }; // SquaredNorm

  /// Range of slots [first, last) of one cell. A slot is a position in
  /// the arrays sorted by cell, sortedIds() and sortedPositions(): the
  /// same slot k gives the id and the cached position of a point, and
  /// the points of a cell occupy consecutive slots. Unlike pointers,
  /// the same range is valid on the host and on a device copy of the
  /// arrays. Iterating it yields the slots (not the point ids):
  ///
  ///   for (auto k : grid.cell(c))
  ///     use(grid.sortedIds()[k], grid.sortedPositions()[k]);
  struct SlotRange
  {
    PointId first;
    PointId last;

    /// Iterator over the slots of a range (a plain counter).
    struct iterator
    {
      using iterator_category = std::forward_iterator_tag;
      using value_type = PointId;
      using difference_type = std::ptrdiff_t;
      using pointer = void;
      using reference = PointId;

      PointId k{};

      PointId operator *() const
      {
        return k;
      }

      iterator& operator ++()
      {
        ++k;
        return *this;
      }

      iterator operator ++(int)
      {
        auto temp = *this;

        ++k;
        return temp;
      }

      bool operator ==(const iterator&) const = default;

    }; // iterator

    iterator begin() const
    {
      return {first};
    }

    iterator end() const
    {
      return {last};
    }

    auto size() const
    {
      return size_t(last - first);
    }

    bool empty() const
    {
      return first == last;
    }

  }; // SlotRange

  static constexpr bool cachesPositions()
  {
    return cachePositions;
  }

  /// Constructs a grid with cells of size h over domain (see
  /// RegionGrid) for points, and calls update().
  PointGrid(const bounds_type& domain, PA& points, R h):
    Base{domain, h},
    PointSet{points}
  {
    update();
  }

  /// Constructs a grid with size cells over domain (see RegionGrid)
  /// for points, and calls update().
  PointGrid(const bounds_type& domain, PA& points, const index_type& size):
    Base{domain, size},
    PointSet{points}
  {
    update();
  }

  /// Constructs a grid with cells of size h over the bounds of points
  /// (a cube, if squared).
  PointGrid(PA& points, R h, bool squared = true):
    PointGrid{PointSet::computeBounds(points, squared), points, h}
  {
    // do nothing
  }

  PointGrid(const PointGrid&) = delete;
  PointGrid& operator =(const PointGrid&) = delete;

  PointGrid(PointGrid&& other) noexcept:
    Base{std::move(other)},
    PointSet{other.points()},
    _ids{std::exchange(other._ids, nullptr)},
    _positions{std::exchange(other._positions, nullptr)},
    _capacity{std::exchange(other._capacity, 0)},
    _count{std::exchange(other._count, 0)},
    _outside{std::exchange(other._outside, 0)}
  {
    // do nothing
  }

  ~PointGrid()
  {
    delete []_ids;
    delete []_positions;
  }

  /// Notifies the grid that the point set changed (points moved,
  /// added or removed) and rebuilds it from the current positions,
  /// in O(n + number of cells). Queries reflect the state of the
  /// last call. No memory is allocated, unless the capacity of the
  /// point set grew since the last call.
  void update();

  /// Rearranges the points so that each id becomes its slot: the
  /// active points get the ids [0, pointCount()), in the order of
  /// sortedIds() (by cell), and the inactive ones (removed points)
  /// the ids [pointCount(), points().size()), by rearrange(points(),
  /// order), with order[k] the id of the point that goes to k, and
  /// then compacts itself, discarding the removed points. Everything
  /// the owner keeps per point must be in the point set. No memory
  /// is allocated: order is the array of sorted ids, which ends as
  /// the identity. Returns pointCount().
  PointId reorder()
    requires requires (PA& pa, std::span<PointId> order)
  {
    rearrange(pa, order);
  };

  /// Returns the number of points stored in the grid (the active
  /// points at the last update()).
  auto pointCount() const
  {
    return _count;
  }

  /// Returns the number of points outside bounds() at the last
  /// update() (stored in the nearest border cells; see the overview).
  auto outsideCount() const
  {
    return _outside;
  }

  /// Returns the approximate memory used by the grid, in bytes.
  size_t memoryUsage() const
  {
    auto s = size_t(this->length() + _capacity) * sizeof(PointId);

    if constexpr (cachePositions)
      s += _capacity * sizeof(vec_type);
    return s;
  }

  /// Iterator over the cells of the grid (in cell id order), whose
  /// value is the SlotRange of the cell; id() and index() give the
  /// cell, as in the iterators of Grid.
  class CellIterator
  {
  public:
    using iterator_concept = std::forward_iterator_tag;
    using iterator_category = std::input_iterator_tag; // value, not ref
    using value_type = SlotRange;
    using difference_type = std::ptrdiff_t;
    using pointer = void;
    using reference = SlotRange;

    CellIterator() = default;

    SlotRange operator *() const
    {
      return _grid->cell(_id);
    }

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

    bool operator ==(const CellIterator& other) const
    {
      assert(_grid == other._grid);
      return _id == other._id;
    }

    auto id() const
    {
      return _id;
    }

    auto index() const
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

  /// Returns the range of slots of a cell (see SlotRange). The grid is
  /// a container of cells: operator [] is a synonym. Note: the argument
  /// is a cell id (or index, or a point of the domain), not a point id;
  /// since both are integers, passing a point id compiles silently.
  SlotRange cell(id_type id) const
  {
    return {cellBegin(id), cellEnd(id)};
  }

  SlotRange cell(const index_type& index) const
  {
    return cell(Base::cellId(index));
  }

  /// Cell containing p, which must be in the domain (see cellIndex(p)).
  SlotRange cell(const vec_type& p) const
  {
    return cell(Base::cellId(p));
  }

  SlotRange operator [](id_type id) const
  {
    return cell(id);
  }

  SlotRange operator [](const index_type& index) const
  {
    return cell(index);
  }

  SlotRange operator [](const vec_type& p) const
  {
    return cell(p);
  }

  /// Iteration over the cells (see CellIterator).
  CellIterator begin() const
  {
    return {this, 0};
  }

  CellIterator end() const
  {
    return {this, this->length()};
  }

  CellIterator cbegin() const
  {
    return begin();
  }

  CellIterator cend() const
  {
    return end();
  }

  /// Raw arrays (e.g., to upload to the GPU).
  const PointId* cellStart() const
  {
    return Base::cells();
  }

  const PointId* sortedIds() const
  {
    return _ids;
  }

  const vec_type* sortedPositions() const
    requires cachePositions
  {
    return _positions;
  }

  /// Calls f(const Neighbor&) for every point q with |p - q| <= radius,
  /// in no particular order. f returns true to continue the search or
  /// false to stop it. Returns the number of calls to f. No memory is
  /// allocated. Each functor type instantiates the search (~0.8 KB of
  /// code); to share a single instantiation among several functors,
  /// pass a type-erased reference (e.g., std::function_ref), at the
  /// cost of an indirect call per neighbor.
  /// For SPH, a cell size equal to the support radius is the best
  /// choice (measured: smaller cells or clipping rows by the sphere
  /// test fewer points, but cost more).
  template <typename F>
  size_t forEachNeighbor(const vec_type& p, R radius, F&& f) const
  {
    return radiusSearch(p, radius, PointId(-1), f);
  }

  /// Same as above, centered at point i and excluding i itself (by id:
  /// other points coincident with i are reported).
  template <typename F>
  size_t forEachNeighbor(PointId i, R radius, F&& f) const
  {
    assert(i >= 0 && size_t(i) < size_t(this->points().size()));
    return radiusSearch(position(i), radius, i, f);
  }

  /// Collects the ids of the points q with |p - q| <= radius, in no
  /// particular order, into out, a buffer of FIXED size: any type with
  /// size() and operator[] returning PointId& (std::array, std::span,
  /// std::vector, ...). The capacity is out.size(): the buffer is
  /// neither resized nor allocated. Returns the total number n of
  /// neighbors found: only out[0, min(n, out.size())) is meaningful,
  /// and n > out.size() means the result was truncated. (The other
  /// slots may be overwritten.) To collect into a growable container,
  /// use forEachNeighbor() instead.
  /// Faster than collecting with forEachNeighbor(): the candidates are
  /// written without branching on the distance test (~85% of them
  /// fail it in an unpredictable pattern), which avoids a branch
  /// misprediction per neighbor (measured ~1.5x).
  template <typename B>
  [[nodiscard]] size_t collectNeighbors(const vec_type& p,
    R radius,
    B& out) const
  {
    return collect(p, radius, PointId(-1), out);
  }

  /// Same as above, centered at point i and excluding i itself.
  template <typename B>
  [[nodiscard]] size_t collectNeighbors(PointId i, R radius, B& out) const
  {
    assert(i >= 0 && size_t(i) < size_t(this->points().size()));
    return collect(position(i), radius, i, out);
  }

  /// Finds the knn.k() nearest points to p with distance (norm value)
  /// <= knn.maxDistance(), stored in knn in ascending distance. Returns
  /// knn.size(). norm is any callable R(const vec_type&) monotone with
  /// the absolute value of each coordinate (e.g., an Lp norm, weighted
  /// or not, or its square); the default is the squared Euclidean
  /// distance. No memory is allocated.
  template <int K, typename N = SquaredNorm>
  int findNearestNeighbors(const vec_type& p,
    knn_type<K>& knn,
    N norm = N{}) const
  {
    return nearestSearch(p, PointId(-1), knn, norm);
  }

  /// Same as above, for point i and excluding i itself (by id).
  template <int K, typename N = SquaredNorm>
  int findNearestNeighbors(PointId i,
    knn_type<K>& knn,
    N norm = N{}) const
  {
    assert(i >= 0 && size_t(i) < size_t(this->points().size()));
    return nearestSearch(position(i), i, knn, norm);
  }

private:
  PointId* _ids{}; // point ids sorted by cell
  vec_type* _positions{}; // their positions (if cachePositions)
  size_t _capacity{}; // size of _ids and _positions
  PointId _count{}; // points stored at the last update()
  PointId _outside{}; // of which outside bounds()

  // Allocates the arrays for capacity points (frees the old ones
  // first: no peak of memory; contents are not kept).
  void allocate(size_t capacity);

  bool isConsistent() const;

  // Cell of p, clamped to the grid; inside is set to contains(p)
  // (computed together, on the same floatIndex(p)).
  id_type bucket(const vec_type& p, bool& inside) const;

  auto position(PointId i) const
  {
    return vec_type{this->points()[i]};
  }

  auto slotPosition(PointId k) const
  {
    if constexpr (cachePositions)
      return _positions[k];
    else
      return position(_ids[k]);
  }

  auto cellBegin(id_type id) const
  {
    return Base::cells()[id];
  }

  auto cellEnd(id_type id) const
  {
    return id + 1 < this->length() ? Base::cells()[id + 1] : _count;
  }

  // Calls f(first, last) with the slot range of the cells of the row
  // c.y (c.z) from x0 to x1: the cells of a row are consecutive, so
  // are their slots. f returns false to stop the visit.
  template <typename F>
  bool visitRow(index_type c, id_type x0, id_type x1, F& f) const
  {
    c.x = x0;

    const auto id = Base::cellId(c);

    return f(cellBegin(id), cellEnd(id + (x1 - x0)));
  }

  // Visits the rows of the cells in [lo, hi] (both valid indices).
  template <typename F>
  bool visitBox(const index_type& lo, const index_type& hi, F& f) const;

  // Visits the cells (in rows) at Chebyshev distance ring from c,
  // clipped to the grid.
  template <typename F>
  bool visitShell(const index_type& c, id_type ring, F& f) const;

  template <typename F>
  size_t radiusSearch(const vec_type& p,
    R radius,
    PointId self,
    F& f) const;

  template <typename B>
  size_t collect(const vec_type& p,
    R radius,
    PointId self,
    B& out) const;

  template <int K, typename N>
  int nearestSearch(const vec_type& p,
    PointId self,
    knn_type<K>& knn,
    N& norm) const;

}; // PointGrid

template <int D, IsReal R, typename PA, typename I, bool C>
void
PointGrid<D, R, PA, I, C>::allocate(size_t capacity)
{
  if (capacity > size_t(std::numeric_limits<PointId>::max()))
    throw std::length_error("PointGrid: too many points");
  delete []_ids;
  delete []_positions;
  _ids = nullptr;
  _positions = nullptr;
  _capacity = 0;
  _count = _outside = 0;
  if (capacity == 0)
    return;
  _ids = new PointId[capacity];
  if constexpr (C)
  {
    try
    {
      _positions = new vec_type[capacity];
    }
    catch (...)
    {
      // Also called by the constructors (via update()), whose failure
      // does not run the destructor
      delete []_ids;
      _ids = nullptr;
      throw;
    }
  }
  _capacity = capacity;
}

template <int D, IsReal R, typename PA, typename I, bool C>
inline auto
PointGrid<D, R, PA, I, C>::bucket(const vec_type& p, bool& inside) const
-> id_type
{
  // Same as clampedCellIndex(p) and contains(p), on one floatIndex(p)
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

template <int D, IsReal R, typename PA, typename I, bool C>
void
PointGrid<D, R, PA, I, C>::update()
{
  const auto& points = this->points();

  if (const auto capacity = internal::pg::pointCapacity(points);
    capacity > _capacity)
    allocate(capacity);

  const auto n = points.size();
  using psize_t = std::remove_const_t<decltype(n)>;

  assert(size_t(n) <= _capacity);

  const auto start = Base::cells();
  const auto length = this->length();
  bool inside;

  // 1. Count the points of each cell (and those outside bounds())
  std::fill_n(start, length, PointId(0));

  PointId count{0};
  PointId outside{0};

  for (psize_t i = 0; i < n; ++i)
    if (this->isPointActive(i))
    {
      ++start[bucket(vec_type{points[i]}, inside)];
      ++count;
      outside += !inside;
    }
  // 2. Inclusive prefix sum: start[c] = end slot of cell c
  {
    PointId sum{0};

    for (id_type c = 0; c < length; ++c)
      start[c] = (sum += start[c]);
  }
  // 3. Scatter backwards: decrementing the end slots leaves start[c]
  // as the first slot of cell c and keeps ids ascending in the cell
  for (psize_t i = n; i-- > 0;)
    if (this->isPointActive(i))
    {
      const vec_type p{points[i]};
      // Same cell as bucket(p) in pass 1 (same computation)
      const auto k = --start[this->clampedCellId(p)];

      _ids[k] = PointId(i);
      if constexpr (C)
        _positions[k] = p;
    }
  _count = count;
  _outside = outside;
}

template <int D, IsReal R, typename PA, typename I, bool C>
bool
PointGrid<D, R, PA, I, C>::isConsistent() const
{
  const auto& points = this->points();
  const auto n = points.size();
  using psize_t = std::remove_const_t<decltype(n)>;
  PointId active{0};

  for (psize_t i = 0; i < n; ++i)
    active += this->isPointActive(i) ? 1 : 0;
  if (active != _count)
    return false;
  for (id_type c = 0, length = this->length(); c < length; ++c)
    for (auto k = cellBegin(c), e = cellEnd(c); k < e; ++k)
    {
      const auto i = _ids[k];

      if (i < 0 || psize_t(i) >= n || !this->isPointActive(i))
        return false;

      const auto p = position(i);

      if (this->clampedCellId(p) != c)
        return false;
      if constexpr (C)
        if (!(_positions[k] == p))
          return false;
    }
  return true;
}

template <int D, IsReal R, typename PA, typename I, bool C>
auto
PointGrid<D, R, PA, I, C>::reorder() -> PointId
  requires requires (PA& pa, std::span<PointId> order)
{
  rearrange(pa, order);
}
{
  assert(isConsistent());

  auto& points = this->points();
  const auto n = PointId(points.size());

  if (n == 0)
    return 0;
  assert(size_t(n) <= _capacity);
  // order[k] = id of the point that goes to slot (new id) k: the
  // sorted ids, then the inactive points
  for (PointId i = 0, k = _count; i < n; ++i)
    if (!this->isPointActive(i))
      _ids[k++] = i;
  rearrange(points, std::span<PointId>{_ids, size_t(n)});
  return _count;
}

template <int D, IsReal R, typename PA, typename I, bool C>
template <typename F>
inline bool
PointGrid<D, R, PA, I, C>::visitBox(const index_type& lo,
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

template <int D, IsReal R, typename PA, typename I, bool C>
template <typename F>
bool
PointGrid<D, R, PA, I, C>::visitShell(const index_type& c,
  id_type ring,
  F& f) const
{
  const auto& n = this->size();
  const auto x0 = math::max(c.x - ring, id_type(0));
  const auto x1 = math::min(c.x + ring, n.x - 1);
  const auto y0 = math::max(c.y - ring, id_type(0));
  const auto y1 = math::min(c.y + ring, n.y - 1);
  // Rows on the faces of the shell are visited entirely; the other
  // rows only have their two end cells on the shell
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

namespace internal::pg
{ // begin namespace internal::pg

template <typename F, typename Neighbor>
concept NeighborFunctor = std::is_invocable_v<F&, const Neighbor&>&&
std::is_same_v<std::invoke_result_t<F&, const Neighbor&>, bool>;

} // end namespace internal::pg

template <int D, IsReal R, typename PA, typename I, bool C>
template <typename F>
size_t
PointGrid<D, R, PA, I, C>::radiusSearch(const vec_type& p,
  R radius,
  PointId self,
  F& f) const
{
  static_assert(internal::pg::NeighborFunctor<F, Neighbor>,
    "PointGrid: functor must be bool(const Neighbor&) "
    "(return true to continue the search, false to stop it)");
  if (!(radius >= 0) || _count == 0) // also rejects NaN
    return 0;

  const auto r2 = radius * radius;
  const auto ids = _ids;
  const vec_type r{radius};
  size_t calls = 0;
  auto test = [&](PointId first, PointId last)
  {
    for (auto k = first; k < last; ++k)
    {
      Neighbor nb;

      nb.d = p - slotPosition(k);
      nb.d2 = nb.d.squaredNorm();
      if (nb.d2 <= r2)
      {
        nb.id = ids[k];
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

namespace internal::pg
{ // begin namespace internal::pg

template <typename B, typename I>
concept IndexBuffer = requires (B & b, size_t i)
{
  { b.size() } -> std::convertible_to<size_t>;
  { b[i] } -> std::same_as<I&>;
};

} // end namespace internal::pg

template <int D, IsReal R, typename PA, typename I, bool C>
template <typename B>
size_t
PointGrid<D, R, PA, I, C>::collect(const vec_type& p,
  R radius,
  PointId self,
  B& out) const
{
  static_assert(internal::pg::IndexBuffer<B, PointId>,
    "PointGrid: buffer must have size() and operator[] returning "
    "PointId&");
  if (!(radius >= 0) || _count == 0) // also rejects NaN
    return 0;

  const auto r2 = radius * radius;
  const auto ids = _ids;
  const auto capacity = size_t(out.size());
  const vec_type r{radius};
  size_t n = 0; // neighbors found (written while n < capacity)
  auto test = [&](PointId first, PointId last)
  {
    auto k = first;

    // Branchless: every candidate is written to out[n] and n advances
    // only if it passes; a rejected one is overwritten by the next
    for (; k < last && n < capacity; ++k)
    {
      const auto id = ids[k];

      out[n] = id;
      n += size_t(((p - slotPosition(k)).squaredNorm() <= r2) & (id != self));
    }
    // Buffer full: only counts
    for (; k < last; ++k)
      n += size_t(((p - slotPosition(k)).squaredNorm() <= r2) &
        (ids[k] != self));
    return true;
  };

  visitBox(this->clampedCellIndex(p - r), this->clampedCellIndex(p + r), test);
  return n;
}

template <int D, IsReal R, typename PA, typename I, bool C>
template <int K, typename N>
int
PointGrid<D, R, PA, I, C>::nearestSearch(const vec_type& p,
  PointId self,
  knn_type<K>& knn,
  N& norm) const
{
  static_assert(std::is_invocable_r_v<R, N&, const vec_type&>,
    "PointGrid: norm must be callable as R(const vec_type&)");
  knn.reset();
  if (_count == 0)
    return 0;

  const auto ids = _ids;
  auto test = [&](PointId first, PointId last)
  {
    for (auto s = first; s < last; ++s)
      if (const auto d = R(norm(p - slotPosition(s))); d <= knn.limit())
        if (const auto id = ids[s]; id != self)
          knn.insert(id, d);
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

    // Any point not visited yet is in a cell outside the box of cells
    // [c - ring, c + ring] (points outside the bounds are in border
    // cells, beyond the outer faces of the box), so it is at least
    // gap away from p along the axis a of some face of the box that
    // is not on the border. By the monotonicity of the norm, its
    // distance is at least norm(gap * e_a).
    auto bound = knn_type<K>::infinity();
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
