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
// OVERVIEW: KNNResult.h
// ========
// Class definition for k-nearest neighbors result.
//
// Author: Paulo Pagliosa
// Last revision: 23/10/2026

#ifndef __KNNResult_h
#define __KNNResult_h

#include <algorithm>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace cg
{ // begin namespace cg

/// What the entries of a KNNResult keep of each neighbor: its point
/// id, its slot in the sorted arrays of the container, or both.
enum class KNNKey
{
  Id,
  Slot,
  IdAndSlot

}; // KNNKey

namespace internal::knn
{ // begin namespace internal::knn

//
// Entry of a KNNResult
//
template <typename I, typename R, KNNKey key> struct Entry;

template <typename I, typename R>
struct Entry<I, R, KNNKey::Id>
{
  I id;
  R distance;

  void set(I i, I, R d)
  {
    id = i;
    distance = d;
  }

}; // Entry

template <typename I, typename R>
struct Entry<I, R, KNNKey::Slot>
{
  I slot;
  R distance;

  void set(I, I k, R d)
  {
    slot = k;
    distance = d;
  }

}; // Entry

template <typename I, typename R>
struct Entry<I, R, KNNKey::IdAndSlot>
{
  I id;
  I slot;
  R distance;

  void set(I i, I k, R d)
  {
    id = i;
    slot = k;
    distance = d;
  }

}; // Entry

//
// Storage of the entries of a KNNResult
//
template <typename E, int K>
struct Storage // K > 0: embedded, trivially copyable, no allocation
{
  E data[K];

  [[nodiscard]] static constexpr auto capacity()
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
    data{allocate(k)},
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
      delete []data;
      data = std::exchange(other.data, nullptr);
      k = std::exchange(other.k, 0);
    }
    return *this;
  }

  ~Storage()
  {
    delete[]data;
  }

  [[nodiscard]] auto capacity() const
  {
    return k;
  }

private:
  static E* allocate(int k)
  {
    if (k < 1)
      throw std::invalid_argument("KNNResult: k must be positive");
    return new E[k]; // default-initialized (not zeroed)
  }

}; // Storage

} // end namespace internal::knn


/////////////////////////////////////////////////////////////////////
//
// KNNResult: k-nearest neighbors result class
// =========
template <typename I, typename R, int K = 0, KNNKey key = KNNKey::Id>
class KNNResult
{
public:
  static_assert(K >= 0, "KNNResult: bad capacity");

  using id_type = I;
  using value_type = R;
  using Entry = internal::knn::Entry<I, R, key>;

  static constexpr bool keepsIds = key != KNNKey::slot;
  static constexpr bool keepsSlots = key != KNNKey::id;

  [[nodiscard]] static constexpr R infinity()
  {
    return std::numeric_limits<R>::infinity();
  }

  /// Constructs an object for (at most) k neighbors.
  explicit KNNResult(int k)
    requires (K == 0) :
    _storage{k}
  {
    // do nothing
  }

  /// Constructs an object for (at most) K neighbors.
  KNNResult()
    requires (K > 0) = default;

  /// Returns the maximum number of neighbors.
  [[nodiscard]] auto k() const
  {
    return _storage.capacity();
  }

  /// Returns the number of neighbors found by the last search
  /// (less than k() if there were not enough points).
  [[nodiscard]] auto size() const
  {
    return _size;
  }

  [[nodiscard]] bool empty() const
  {
    return _size == 0;
  }

  [[nodiscard]] bool full() const
  {
    return _size == k();
  }

  /// Returns the i-th nearest neighbor (ascending distance).
  [[nodiscard]] const auto& operator [](int i) const
  {
    assert(i >= 0 && i < _size);
    return data()[i];
  }

  /// Returns the point id of the i-th nearest neighbor.
  [[nodiscard]] auto id(int i) const
    requires keepsIds
  {
    return (*this)[i].id;
  }

  /// Returns the slot of the i-th nearest neighbor.
  [[nodiscard]] auto slot(int i) const
    requires keepsSlots
  {
    return (*this)[i].slot;
  }

  [[nodiscard]] auto distance(int i) const
  {
    return (*this)[i].distance;
  }

  [[nodiscard]] const auto* begin() const
  {
    return data();
  }

  [[nodiscard]] const auto* end() const
  {
    return data() + _size;
  }

  /// Returns the maximum distance of a neighbor. Searches find
  /// the k nearest points with with distance <= maxDistance().
  [[nodiscard]] auto maxDistance() const
  {
    return _maxDistance;
  }

  /// Sets the maximum distance (for the next search).
  void setMaxDistance(R d)
  {
    _maxDistance = d >= 0 ? d : R(0); // NaN -> 0
  }

  /// Starts a new search (keeps k and the maximum distance).
  void reset()
  {
    _size = 0;
    _limit = _maxDistance;
  }

  /// Returns a bound for a quick rejection: a point at distance
  /// d > limit() cannot be kept (cheap test for the inner loops;
  /// the exact test is accepts()).
  [[nodiscard]] auto limit() const
  {
    return _limit;
  }

  /// Returns true if a point at distance d would be kept: while
  /// not full, d <= maxDistance(); after, d < distance of the
  /// farthest.
  [[nodiscard]] bool accepts(R d) const
  {
    return d < _limit || (d == _limit && !full());
  }

  /// Offers a candidate (point id, slot and distance); returns
  /// true if it was kept. The entries keep what key says.
  bool insert(I id, I slot, R d);

  /// Offers a candidate (point id and distance).
  bool insert(I id, R d)
    requires (key == KNNKey::id)
  {
    return insert(id, I{}, d);
  }

  /// Finishes the search. Nothing to do: the neighbors are kept
  /// sorted by insertion.
  void finish()
  {
    // do nothing
  }

private:
  internal::knn::Storage<Entry, K> _storage;
  int _size{};
  R _limit{infinity()};
  R _maxDistance{infinity()};

  const auto* data() const
  {
    return _storage.data;
  }

  auto* data()
  {
    return _storage.data;
  }

}; // KNNResult

template <typename I, typename R, int K, KNNKey key>
inline bool
KNNResult<I, R, K, key>::insert(I id, I slot, R d)
{
  if (!accepts(d))
    return false;

  const auto e = data();
  // Sorted insertion; when full, the farthest is dropped
  int j = full() ? _size - 1 : _size++;

  for (; j > 0 && e[j - 1].distance > d; --j)
    e[j] = e[j - 1];
  e[j].set(id, slot, d);
  if (full())
    _limit = e[_size - 1].distance;
  return true;
}

} // end namespace cg

#endif // __KNNResult_h
