//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2022 Paulo Pagliosa.                        |
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
// OVERVIEW: ElevationFilter.h
// ========
// Class definition for vis elevation filter.
//
// Author: Paulo Pagliosa
// Last revision: 10/03/2022

#ifndef __ElevationFilter_h
#define __ElevationFilter_h

#include "Filter.h"
#include "Scalars.h"

namespace cg::vis
{ // begin namespace cg::vis

enum class ElevationDirection
{
  X,
  Y,
  Z,
  Points
};


/////////////////////////////////////////////////////////////////////
//
// ElevationFilter: vis elevation filter class
// ===============
template <typename T>
class ElevationFilter final: public Filter<T, T>
{
public:
  static Reference<ElevationFilter<T>> New()
  {
    return new ElevationFilter<T>;
  }

  auto minScalar() const
  {
    return _scalarRange[0];
  }

  auto maxScalar() const
  {
    return _scalarRange[1];
  }

  void setScalarRange(float min, float max);

  auto minPoint() const
  {
    return _p1;
  }

  auto maxPoint() const
  {
    return _p2;
  }

  void setPoints(const vec3f& p1, const vec3f& p2);

  void setDirection(ElevationDirection direction)
  {
    _direction = direction;
  }

private:
  ElevationDirection _direction;
  float _scalarRange[2];
  vec3f _p1;
  vec3f _p2;

  ElevationFilter():
    _direction{ElevationDirection::Points},
    _p1{0, 0, 0},
    _p2{0, 0, 1}
  {
    _scalarRange[0] = 0;
    _scalarRange[1] = 1;
  }

  void execute() override;

}; // ElevationFilter

template <typename T>
void
ElevationFilter<T>::setScalarRange(float min, float max)
{
  _scalarRange[0] = min;
  _scalarRange[1] = max;
  this->modified();
}

template <typename T>
void
ElevationFilter<T>::setPoints(const vec3f& p1, const vec3f& p2)
{
  _p1 = p1;
  _p2 = p2;
  if ((p2 - p1).isNull())
    _p2.z += 1;
  this->modified();
}

template <typename T>
void
ElevationFilter<T>::execute()
{
  auto input = this->input();
  auto output = Object::makeCopy(input);

  if (output == nullptr)
    throw std::runtime_error("ElevationFilter::execute(): no output");
  this->setOutput(output);

  auto nv = input->vertexCount();

  if (nv < 1)
    return;

  vec3f p1;
  vec3f p2;

  if (_direction == ElevationDirection::Points)
  {
    p1 = _p1;
    p2 = _p2;
  }
  else
  {
    auto d = (int)_direction;
    auto b = input->bounds();
    auto s = b.size()[d] * 0.5f;

    p1 = p2 = b.center();
    p1[d] -= s;
    p2[d] += s;
  }

  auto scalars = new Scalars{nv};
  auto dv = p2 - p1;
  auto dl = math::inverse(dv.dot(dv));
  auto ds = _scalarRange[1] - _scalarRange[0];

  for (decltype(nv) i = 0; i < nv; ++i)
  {
    const auto v = input->vertex(i) - p1;
    auto s = v.dot(dv) * dl;

    if (s < 0)
      s = 0;
    else if (s > 1)
      s = 1;
    scalars->set(i, _scalarRange[0] + s * ds);
  }
  output->setVertexScalars(scalars);
#if _DEBUG
  puts("ELEVATION FILTER EXECUTED");
#endif // _DEBUG
}

} // end namespace cg::vis

#endif // __ElevationFilter_h
