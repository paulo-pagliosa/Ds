//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2016, 2019 Orthrus Group.                         |
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
// OVERVIEW: KNN.h
// ========
// Class definition for KNN helper.
//
// Author: Paulo Pagliosa
// Last revision: 28/02/2019

#ifndef __KNNHelper_h
#define __KNNHelper_h

#include <functional>
#include <limits>

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// KNNHelper: KNNHelper class
// =========
template <typename Vector> class KNNHelper
{
public:
  using real = typename Vector::value_type;
  using Norm = std::function<real(const Vector&)>;

  template <typename V>
  class Queue
  {
  public:
    Queue(int k):
      _k{k},
      _n{0},
      _entries{new Entry[k + 1]}
    {
      // do nothing
    }

    ~Queue()
    {
      delete []_entries;
    }

    real key(int i) const
    {
      return _entries[i].key;
    }

    const V& value(int i) const
    {
      return _entries[i].value;
    }

    real maxKey() const
    {
      return key(_n);
    }

    int size() const
    {
      return _n;
    }

    bool insert(real key, const V& value)
    {
      if (key >= this->key(_k))
        return false;

      int i;

      for (i = _n; i > 0; --i)
      {
        if (_entries[i - 1].key > key)
          _entries[i] = _entries[i - 1];
        else
          break;
      }
      _entries[i].key = key;
      _entries[i].value = value;
      if (_n < _k)
        ++_n;
      return true;
    }

  private:
    struct Entry
    {
      real key;
      V value;

      Entry():
        key{std::numeric_limits<real>::max()}
      {
        // do nothing
      }

    }; // Entry

    int _k;
    int _n;
    Entry* _entries;

  }; // Queue

  static real squaredNorm(const Vector& p)
  {
    return p.squaredNorm();
  }

  KNNHelper(const Vector& p, int k, Norm norm = squaredNorm):
    _sample{p},
    _queue(k),
    _norm{norm}
  {
    // do nothing
  }

  void setNorm(Norm norm)
  {
    _norm = norm ? norm : squaredNorm;
  }

  bool test(const Vector&p, int id)
  {
    return _queue.insert(_norm(_sample - p), id);
  }

  bool test(const Vector&p, int id, real maxd)
  {
    auto d = _norm(_sample - p);
    return d < maxd ? _queue.insert(d, id) : false;
  }

  const Vector& sample() const
  {
    return _sample;
  }

  real maxSquaredDistance() const
  {
    return _queue.maxKey();
  }

  int results(int indices[], real* distances = nullptr) const
  {
    int k = _queue.size();

    if (distances == nullptr)
      for (int i = 0; i < k; ++i)
        indices[i] = _queue.value(i);
    else
      for (int i = 0; i < k; ++i)
      {
        indices[i] = _queue.value(i);
        distances[i] = _queue.key(i);
      }
    return k;
  }

private:
  Vector _sample;
  Queue<int> _queue;
  Norm _norm;

}; // KNNHelper

} // namespace cg

#endif // __KNNHelper_h
