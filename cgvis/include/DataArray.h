//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022 Paulo Pagliosa.                              |
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
// OVERVIEW: DataArray.h
// ========
// Class definition for generic vis data array.
//
// Author: Paulo Pagliosa
// Last revision: 14/03/2022

#ifndef __DataArray_h
#define __DataArray_h

#include "Object.h"
#include <vector>

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// DataArray: generic vis data array class
// =========
template <typename T>
class DataArray: public virtual Object
{
public:
  using data_type = T;

  DataArray(uint32_t size = 0):
    _data(size)
  {
    modified();
  }

  auto size() const
  {
    return (uint32_t)_data.size();
  }

  auto data() const
  {
    return _data.data();
  }

  const auto& get(int i) const
  {
    return _data[i];
  }

  auto begin() const
  {
    return _data.begin();
  }

  auto end() const
  {
    return _data.end();
  }

  void add(const T& value)
  {
    _data.push_back(value);
    _computeTime.reset();
  }

  void set(int i, const T& value);

protected:
  std::vector<T> _data;
  Timestamp _computeTime;

}; // DataArray

template <typename T>
void
DataArray<T>::set(int i, const T& value)
{
  auto d = i - (int)size();

  if (d < 0)
    _data[i] = value;
  else if (d == 0)
    _data.push_back(value);
  else
  {
    _data.resize((size_t)i + 1);
    _data[i] = value;
  }
  _computeTime.reset();
}

// Data array of void
template <>
class DataArray<void>
{
public:
  using data_type = void;

}; // DataArray

} // end namespace cg::vis

#endif // __DataArray_h
