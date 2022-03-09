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
// OVERVIEW: Glyph3.h
// ========
// Class definition for vis 3D glyph filter.
//
// Author: Paulo Pagliosa
// Last revision: 08/03/2022

#ifndef __Glyph3_h
#define __Glyph3_h

#include "Filter.h"
#include "PolyData.h"

namespace cg::vis
{ // begin namespace cg::vis


/////////////////////////////////////////////////////////////////////
//
// Glyph3Base: vis 3D glyph base class
// ==========
class Glyph3Base: public virtual Object
{
public:
  enum class ScaleMode
  {
    Scalar,
    Vector
  };

  constexpr static auto minScaleFactor = 0.001f;

  PolyData* source() const
  {
    return _source;
  }

  void setSource(const PolyData* source)
  {
    if (_source != source)
    {
      _source = source;
      modified();
    }
  }

  void range(float& min, float& max) const
  {
    min = _range[0];
    max = _range[1];
  }

  void setRange(float min, float max);

  auto scaleFactor() const
  {
    return _scaleFactor;
  }

  void setScaleFactor(float value)
  {
    _scaleFactor = math::max(minScaleFactor, value);
    modified();
  }

  auto scaleMode() const
  {
    return _scaleMode;
  }

  void setScaleMode(ScaleMode scaleMode)
  {
    if (scaleMode != _scaleMode)
    {
      _scaleMode = scaleMode;
      modified();
    }
  }

protected:
  using Points = std::vector<vec3f>;

  Glyph3Base();

  PolyData* makeDefaultSource() const;
  void execute(const Points&, PolyData&);

private:
  Reference<PolyData> _source;
  ScaleMode _scaleMode;
  float _range[2];
  float _scaleFactor;
  bool _clamping;

}; // Glyph3Base


/////////////////////////////////////////////////////////////////////
//
// Glyph3: vis 3D glyph filter class
// ======
template <typename Input>
class Glyph3: public Filter<Input, PolyData>, public Glyph3Base
{
public:
  static Reference<Glyph3<Input>> New()
  {
    return new Glyph3<Input>;
  }

  void execute() override;

private:
  Glyph3() = default;

}; // Glyph3

template <typename Input>
void
Glyph3<Input>::execute()
{
  auto output = new PolyData;

  this->setOutput(output);

  auto input = this->input();
  auto nv = input->vertexCount();

  if (nv == 0)
    return;

  Points points(nv);

  for (decltype(nv) i = 0; i < nv; ++i)
    points[i] = input->vertex(i);
  Glyph3Base::execute(points, *output);
}

} // end namespace cg::vis

#endif
