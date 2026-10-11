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
// OVERVIEW: KNearest.h
// ========
// Class definition for k-nearest neighbors result.
//
// Author: Paulo Pagliosa
// Last revision: 07/10/2026
//
// REVIEW-PUSH: new file (review, 07/10/2026). Result of a k-nearest
// neighbors search. It owns its storage, so the caller cannot pass a
// buffer of the wrong size, and it is reused across queries (no
// allocation per query):
//
//   KNearest<int, float> knn{16};   // k given at run time (one alloc)
//   KNearest<int, float, 16> knn;   // k fixed at compile time (no allocation)
//
//   grid.findNearestNeighbors(p, knn);
//   for (auto [id, d] : knn) ...     // ascending distance
//
// "Distance" means the value of the norm used in the search: with the
// default norm of the containers, it is the SQUARED Euclidean
// distance.
//
// Storage (09/10/2026): no std::unique_ptr (not available on GPUs).
// KNearest<I, R, K> with K > 0 embeds its entries: trivially copyable,
// no allocation, usable on a device. With K == 0, it owns a block
// allocated on the host: move-only; a moved-from object can only be
// destroyed or assigned to.

#ifndef __KNearest_h
#define __KNearest_h

#include <algorithm>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace cg
{ // begin namespace cg

namespace internal::knn
{ // begin namespace internal::knn

//
// Storage of the entries of a KNearest
//
template <typename E, int K>
struct Storage // K > 0: embedded, trivially copyable, no allocation
{
  E data[K];

  static constexpr int capacity()
  {
    return K;
  }

}; // Storage

template <typename E>
struct Storage<E, 0> // K == 0: owns a block allocated on the host
{
  E* data{};
  int k{};

  explicit Storage(int k):
    data{alloc(k)},
    k{k}
  {
    // do nothing
  }

  Storage(const Storage&) = delete;
  Storage& operator =(const Storage&) = delete;

  Storage(Storage&& other) noexcept:
    data{std::exchange(other.data, nullptr)},
    k{std::exchange(other.k, 0)}
  {
    // do nothing
  }

  Storage& operator =(Storage&& other) noexcept
  {
    if (this != &other)
    {
      delete[]data;
      data = std::exchange(other.data, nullptr);
      k = std::exchange(other.k, 0);
    }
    return *this;
  }

  ~Storage()
  {
    delete[]data;
  }

  int capacity() const
  {
    return k;
  }

private:
  static E* alloc(int k)
  {
    if (k < 1)
      throw std::invalid_argument("KNearest: k must be positive");
    return new E[k]; // default-initialized (not zeroed)
  }

}; // Storage

} // end namespace internal::knn


/////////////////////////////////////////////////////////////////////
//
// KNearest: k-nearest neighbors result class
// ========
template <typename I, typename R, int K = 0>
class KNearest
{
public:
  static_assert(K >= 0, "KNearest: bad capacity");

  using id_type = I;
  using value_type = R;

  struct Entry
  {
    I id;
    R distance;

  }; // Entry

  static constexpr R infinity()
  {
    return std::numeric_limits<R>::infinity();
  }

  /// Constructs an object for (at most) k neighbors (allocated on the
  /// host; throws std::invalid_argument if k < 1).
  explicit KNearest(int k)
    requires (K == 0) :
    _storage{k}
  {
    // do nothing
  }

  /// Constructs an object for (at most) K neighbors.
  KNearest()
    requires (K > 0) = default;

  /// Returns the maximum number of neighbors.
  int k() const
  {
    return _storage.capacity();
  }

  /// Returns the number of neighbors found by the last search (less
  /// than k() if there were not enough points).
  int size() const
  {
    return _size;
  }

  bool empty() const
  {
    return _size == 0;
  }

  bool full() const
  {
    return _size == k();
  }

  /// Returns the i-th nearest neighbor (ascending distance).
  const Entry& operator [](int i) const
  {
    assert(i >= 0 && i < _size);
    return data()[i];
  }

  I id(int i) const
  {
    return (*this)[i].id;
  }

  R distance(int i) const
  {
    return (*this)[i].distance;
  }

  const Entry* begin() const
  {
    return data();
  }

  const Entry* end() const
  {
    return data() + _size;
  }

  /// Returns the maximum distance (in norm units) of a neighbor;
  /// infinity by default. Searches find the k nearest points with
  /// distance <= maxDistance().
  R maxDistance() const
  {
    return _maxDistance;
  }

  /// Sets the maximum distance (takes effect on the next search).
  void setMaxDistance(R d)
  {
    _maxDistance = d >= 0 ? d : R(0); // NaN -> 0
  }

  //
  // Search interface (used by the spatial containers)
  //

  /// Starts a new search (keeps k and the maximum distance).
  void reset()
  {
    _size = 0;
    _limit = _maxDistance;
  }

  /// Returns a bound for a quick rejection: a point at distance
  /// d > limit() cannot be kept (cheap test for the inner loops; the
  /// exact test is accepts()).
  R limit() const
  {
    return _limit;
  }

  /// Returns true if a point at distance d would be kept: while not
  /// full, d <= maxDistance(); after, d < distance of the farthest.
  bool accepts(R d) const
  {
    return d < _limit || (d == _limit && !full());
  }

  /// Offers a candidate; returns true if it was kept.
  bool insert(I id, R d);

  /// Finishes the search. Nothing to do: the neighbors are kept sorted
  /// by insertion (measured faster than a heap even for k = 256, since
  /// the containers visit the candidates roughly by distance, so most
  /// insertions shift few entries).
  void finish()
  {
    // do nothing
  }

private:
  internal::knn::Storage<Entry, K> _storage;
  int _size{};
  R _maxDistance{infinity()};
  R _limit{infinity()}; // maxDistance or, if full, the farthest distance

  const Entry* data() const
  {
    return _storage.data;
  }

  Entry* data()
  {
    return _storage.data;
  }

}; // KNearest

template <typename I, typename R, int K>
inline bool
KNearest<I, R, K>::insert(I id, R d)
{
  if (!accepts(d))
    return false;

  const auto e = data();
  // Sorted insertion; when full, the farthest is dropped
  int j = full() ? _size - 1 : _size++;

  for (; j > 0 && e[j - 1].distance > d; --j)
    e[j] = e[j - 1];
  e[j] = {id, d};
  if (full())
    _limit = e[_size - 1].distance;
  return true;
}

} // end namespace cg

#endif // __KNearest_h