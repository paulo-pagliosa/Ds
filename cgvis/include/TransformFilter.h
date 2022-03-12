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
// OVERVIEW: TransformFilter.h
// ========
// Class definition for vis transform filter.
//
// Author: Paulo Pagliosa
// Last revision: 10/03/2022

#ifndef __TransformFilter_h
#define __TransformFilter_h

#include "Filter.h"
#include "Transform.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// TransformFilter: vis transform filter class
// ===============
template <typename T>
class TransformFilter final: public Filter<T, T>
{
public:
  static Reference<TransformFilter<T>> New()
  {
    return new TransformFilter<T>;
  }

  const auto& position() const
  {
    return _position;
  }

  void setPosition(const vec3f& position)
  {
    _position = position;
  }

  const auto& rotation() const
  {
    return _rotation;
  }

  void setRotation(const quatf& rotation)
  {
    _rotation = rotation;
  }

  void setRotation(const vec3f& angles)
  {
    _rotation = quatf::eulerAngles(angles);
  }

  const auto& scale() const
  {
    return _scale;
  }

  void setScale(const vec3f& scale)
  {
    _scale = scale;
  }

  void setScale(float scale)
  {
    setScale(vec3f{scale});
  }

private:
  vec3f _position{0.0f};
  quatf _rotation{quatf::identity()};
  vec3f _scale{1.0f};

  void execute() override;

}; // TransformFilter

template <typename T>
void
TransformFilter<T>::execute()
{
  auto input = this->input();
  auto output = Object::makeCopy(input);

  if (output == nullptr)
    throw std::runtime_error("TransformFilter::execute(): no output");
  this->setOutput(output);
  output->setTransform(_position, _rotation, _scale);
#if _DEBUG
  puts("TRANSFORM FILTER EXECUTED");
#endif // _DEBUG
}

} // end namespace cg::vis

#endif // __TransformFilter_h
