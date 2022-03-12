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
// OVERVIEW: ContoutFilter.h
// ========
// Class definition for vis contour filter.
//
// Author: Paulo Pagliosa
// Last revision: 11/03/2022

#ifndef __ContourFilter_h
#define __ContourFilter_h

#include "Filter.h"
#include "PolyData.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// ContourFilter: vis contour filter class
// =============
template <typename Input>
class ContourFilter final: public Filter<Input, PolyData>
{
public:
  static Reference<ContourFilter<Input>> New()
  {
    return new ContourFilter<Input>;
  }

  ContourFilter()
  {
    _range[0] = -math::Limits<float>::inf();
    _range[1] = +math::Limits<float>::inf();
  }

  void addValue(float value);
  void generateValues(uint32_t n, float min, float max);

  auto valueCount() const
  {
    return (uint32_t)_values.size();
  }

protected:
  std::vector<float> _values;
  float _range[2];

  void execute() override;

}; // ContourFilter

template <typename Input>
void
ContourFilter<Input>::addValue(float value)
{
  _values.push_back(value);
  if (value < _range[0])
    _range[0] = value;
  if (value > _range[1])
    _range[1] = value;
}

template <typename Input>
void
ContourFilter<Input>::generateValues(uint32_t n, float min, float max)
{
  if (n < 2)
    n = 2;
  _values.resize(n);

  auto d = ((_range[1] = max) - (_range[0] = min)) / (n - 1);

  for (decltype(n) i = 0; i < n; i++, min += d)
    _values[i] = min;
}

template <typename Input>
void
ContourFilter<Input>::execute()
{
  auto output = PolyData::New();

  this->setOutput(output);

  auto input = this->input();
  auto scalars = input->vertexScalars();

  if (scalars == nullptr)
    return;
  output->setVertexScalars(new Scalars);
  output->initializeLocator(input->bounds(), 10);

  auto nc = input->cellCount();

  for (decltype(nc) i = 0; i < nc; ++i)
  {
    auto cell = input->cell(i);
    auto nv = valueCount();

    for (decltype(nv) i = 0; i < nv; ++i)
      cell->contour(_values[i], scalars, *output);
  }
  output->deleteLocator();
#ifdef _DEBUG
  puts("CONTOUR FILTER EXECUTED");
#endif // _DEBUG
}

} // end namespace cg::vis

#endif // __ContourFilter_h
